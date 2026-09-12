#include "PresetManager.h"
#include "BinaryData.h"

PresetManager::PresetManager(GuitarToolsAudioProcessor& processor, juce::ComboBox& presetBox) : audioProcessor(processor),
presetBox(presetBox)
{
    
}

void PresetManager::savePreset()
{
    const auto userPresetFolder = getPresetFolder().getChildFile ("User Presets");
    userPresetFolder.createDirectory();
    
    fileChooser = std::make_unique<juce::FileChooser> ("Save Preset", userPresetFolder, "*.xml");
    
    auto fileChooserFlags = juce::FileBrowserComponent::saveMode;
    
    fileChooser->launchAsync (fileChooserFlags, [this, userPresetFolder] (const juce::FileChooser& fileChooser)
    {
        juce::File selectedFile = fileChooser.getResult();

        if (selectedFile == juce::File())
        {
            if (userPresetFolder.findChildFiles (juce::File::findFiles, false).isEmpty())
                userPresetFolder.deleteFile();

            return;
        }
        
        if (selectedFile.existsAsFile() || selectedFile.create())
        {
            // Save the preset to the selected file
            audioProcessor.savePreset(selectedFile);
            DBG("Preset saved to: " + selectedFile.getFullPathName());
            
            // Optionally, refresh the preset list in the GUI or do other actions
            refreshPresetList();
            audioProcessor.setCurrentPresetName (selectedFile.getFileNameWithoutExtension());
            presetBox.setSelectedId (presetFiles.indexOf (selectedFile) + 1, juce::dontSendNotification);
        }
        else
        {
            DBG("Error: Failed to save preset. File creation failed.");
        };
        
    });
}

void PresetManager::loadPreset()
{
    juce::FileChooser chooser ("Load Preset", juce::File::getSpecialLocation (juce::File::userDocumentsDirectory), "*.xml");

    auto file = chooser.getResult();
    audioProcessor.loadPreset (file);  // Calls your processor’s loadPreset
    
    applyPresetSelection(file.getFileNameWithoutExtension());
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

    if (const auto defaultPresetFile = presetFolder.getChildFile ("Default.xml"); defaultPresetFile.existsAsFile())
    {
        presetFiles.add (defaultPresetFile);
        presetBox.addItem (defaultPresetFile.getFileNameWithoutExtension(), presetFiles.size());
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

        const auto presetFile = (originalFilename == "Default.xml" ? presetFolder : factoryPresetFolder).getChildFile (originalFilename);

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
