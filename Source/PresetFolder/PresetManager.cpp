#include "PresetManager.h"
#include "BinaryData.h"

static const juce::StringArray rootPresetFileNames {"Start From Here.xml", "Reset To Default.xml"};

PresetManager::PresetManager(GuitarToolsAudioProcessor& processor) : audioProcessor(processor)
{
    
}

void PresetManager::savePreset(const juce::String& presetName)
{
    const auto presetFile = getUserPresetFile (presetName);
    const auto legalPresetName = presetFile.getFileNameWithoutExtension();

    if (legalPresetName.isEmpty())
        return;

    presetFile.getParentDirectory().createDirectory();
    audioProcessor.savePreset (presetFile);

    audioProcessor.setCurrentPresetName (legalPresetName);

    if (onPresetNameChanged != nullptr)
        onPresetNameChanged();
}

bool PresetManager::deletePreset(const juce::File& presetFile)
{
    if (! presetFile.existsAsFile() || ! presetFile.deleteFile())
        return false;

    if (audioProcessor.getCurrentPresetName() == presetFile.getFileNameWithoutExtension())
        audioProcessor.setCurrentPresetName ({});

    if (onPresetNameChanged != nullptr)
        onPresetNameChanged();

    return true;
}

void PresetManager::loadPresetFile(const juce::File& presetFile)
{
    if (! presetFile.existsAsFile())
        return;

    audioProcessor.loadPreset (presetFile);
    audioProcessor.setCurrentPresetName (presetFile.getFileNameWithoutExtension());

    if (onPresetNameChanged != nullptr)
        onPresetNameChanged();
}

void PresetManager::loadAdjacentPreset(const int indexOffset)
{
    auto presetFiles = getPresetFiles (PresetType::factory);
    presetFiles.addArray (getPresetFiles (PresetType::user));

    const auto numPresets = presetFiles.size();

    if (numPresets == 0)
        return;

    const auto currentPresetName = getCurrentPresetName();
    auto currentIndex = -1;

    for (int fileIndex = 0; fileIndex < numPresets; ++fileIndex)
    {
        if (presetFiles[fileIndex].getFileNameWithoutExtension() == currentPresetName)
        {
            currentIndex = fileIndex;
            break;
        }
    }

    const auto adjacentIndex = currentIndex < 0 ? (indexOffset > 0 ? 0 : numPresets - 1)
                                                : (currentIndex + indexOffset + numPresets) % numPresets;
    loadPresetFile (presetFiles[adjacentIndex]);
}

juce::Array<juce::File> PresetManager::getPresetFiles(const PresetType presetType) const
{
    juce::Array<juce::File> files;
    const auto presetFolder = getPresetFolder();

    if (presetType == PresetType::user)
    {
        files = presetFolder.getChildFile ("User Presets").findChildFiles (juce::File::findFiles, false, "*.xml");
        files.sort();
        return files;
    }

    for (const auto& rootPresetFileName : rootPresetFileNames)
        if (const auto rootPresetFile = presetFolder.getChildFile (rootPresetFileName); rootPresetFile.existsAsFile())
            files.add (rootPresetFile);

    auto factoryPresetFiles = presetFolder.getChildFile ("Factory Presets").findChildFiles (juce::File::findFiles, false, "*.xml");
    factoryPresetFiles.sort();
    files.addArray (factoryPresetFiles);

    return files;
}

juce::File PresetManager::getUserPresetFile(const juce::String& presetName) const
{
    return getPresetFolder().getChildFile ("User Presets").getChildFile (juce::File::createLegalFileName (presetName.trim()) + ".xml");
}

juce::String PresetManager::getCurrentPresetName() const
{
    return audioProcessor.getCurrentPresetName();
}

juce::File PresetManager::getPresetFolder()
{
    return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
          #if JUCE_MAC
           .getChildFile ("Application Support")
          #endif
           .getChildFile (JucePlugin_Manufacturer)
           .getChildFile (JucePlugin_Name)
           .getChildFile ("Presets");
}

void PresetManager::installFactoryPresets()
{
    const auto presetFolder = getPresetFolder();
    const auto factoryPresetFolder = presetFolder.getChildFile ("Factory Presets");
    factoryPresetFolder.createDirectory();

    for (int resourceIndex = 0; resourceIndex < BinaryData::namedResourceListSize; ++resourceIndex)
    {
        const juce::String originalFilename { BinaryData::originalFilenames[resourceIndex] };

        if (! originalFilename.endsWithIgnoreCase (".xml"))
            continue;

        const auto presetFile = (rootPresetFileNames.contains (originalFilename) ? presetFolder : factoryPresetFolder).getChildFile (originalFilename);

        if (presetFile.existsAsFile())
            continue;

        int presetDataSize = 0;

        if (const auto* presetData = BinaryData::getNamedResource (BinaryData::namedResourceList[resourceIndex], presetDataSize))
            presetFile.replaceWithData (presetData, (size_t) presetDataSize);
    }
}
