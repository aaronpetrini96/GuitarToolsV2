#include "PresetManager.h"
#include "BinaryData.h"

static const juce::StringArray rootPresetFileNames {"Start From Here.xml", "Reset To Default.xml"};

PresetManager::PresetManager(GuitarToolsAudioProcessor& processor, juce::ComboBox& presetBox) : audioProcessor(processor),
presetBox(presetBox)
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

    refreshPresetList();
    audioProcessor.setCurrentPresetName (legalPresetName);
    displayCurrentPresetName();
}

bool PresetManager::deletePreset(const juce::File& presetFile)
{
    if (! presetFile.existsAsFile() || ! presetFile.deleteFile())
        return false;

    if (audioProcessor.getCurrentPresetName() == presetFile.getFileNameWithoutExtension())
        audioProcessor.setCurrentPresetName ({});

    refreshPresetList();
    displayCurrentPresetName();
    return true;
}

void PresetManager::loadPresetFile(const juce::File& presetFile)
{
    if (! presetFile.existsAsFile())
        return;

    audioProcessor.loadPreset (presetFile);
    audioProcessor.setCurrentPresetName (presetFile.getFileNameWithoutExtension());
    displayCurrentPresetName();
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

void PresetManager::presetSelected()
{
    const auto selectedId = presetBox.getSelectedId();

    if (! juce::isPositiveAndNotGreaterThan (selectedId, presetFiles.size()))
        return;

    const auto presetFile = presetFiles[selectedId - 1];
    audioProcessor.loadPreset (presetFile);
    audioProcessor.setCurrentPresetName (presetFile.getFileNameWithoutExtension());
}

void PresetManager::refreshPresetList()
{
    presetBox.clear (juce::dontSendNotification);
    presetFiles.clear();

    const auto presetFolder = getPresetFolder();

    for (const auto& rootPresetFileName : rootPresetFileNames)
    {
        if (const auto rootPresetFile = presetFolder.getChildFile (rootPresetFileName); rootPresetFile.existsAsFile())
        {
            presetFiles.add (rootPresetFile);
            presetBox.addItem (rootPresetFile.getFileNameWithoutExtension(), presetFiles.size());
        }
    }

    addPresetFolderToMenu (presetFolder.getChildFile ("Factory Presets"));
    addPresetFolderToMenu (presetFolder.getChildFile ("User Presets"));
}

void PresetManager::addPresetFolderToMenu (const juce::File& folder)
{
    auto folderPresetFiles = folder.findChildFiles (juce::File::findFiles, false, "*.xml");

    if (folderPresetFiles.isEmpty())
        return;

    folderPresetFiles.sort();

    juce::PopupMenu folderMenu;

    for (const auto& presetFile : folderPresetFiles)
    {
        presetFiles.add (presetFile);
        folderMenu.addItem (presetFiles.size(), presetFile.getFileNameWithoutExtension());
    }

    presetBox.getRootMenu()->addSubMenu (folder.getFileName(), folderMenu);
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

int PresetManager::getItemIdForText(const juce::ComboBox& box, const juce::String& textToFind)
{
    for (int i = 0; i < box.getNumItems(); ++i)
    {
        if (box.getItemText(i) == textToFind)
            return box.getItemId(i);
    }
    return 0; // 0 means "not found"
}

void PresetManager::applyPresetSelection(const juce::String& presetName)
{
    audioProcessor.setCurrentPresetName(presetName);
    presetBox.setSelectedId(getItemIdForText(presetBox, presetName));
}


void PresetManager::displayCurrentPresetName()
{
    auto presetName = audioProcessor.getCurrentPresetName();
    if (presetName.isNotEmpty())
    {
        int id = getItemIdForText(presetBox, presetName);
        if (id != 0)
            presetBox.setSelectedId(id, juce::dontSendNotification);
        else
            presetBox.setText(presetName, juce::dontSendNotification);
    }
}
