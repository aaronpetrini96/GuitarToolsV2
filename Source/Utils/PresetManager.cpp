#include "PresetManager.h"

PresetManager::PresetManager(GuitarToolsAudioProcessor& processor, juce::ComboBox& presetBox) : audioProcessor(processor),
presetBox(presetBox)
{
    
}

void PresetManager::savePreset()
{
    auto presetFolder = getPresetFolder();
    if (!presetFolder.exists())
        presetFolder.createDirectory();
    
    fileChooser = std::make_unique<juce::FileChooser> ("Save Preset", presetFolder, "*.xml");
    
    auto fileChooserFlags = juce::FileBrowserComponent::saveMode;
    
    fileChooser->launchAsync (fileChooserFlags, [this] (const juce::FileChooser& fileChooser)
    {
        juce::File selectedFile = fileChooser.getResult();
        
        if (selectedFile.existsAsFile() || selectedFile.create())
        {
            // Save the preset to the selected file
            audioProcessor.savePreset(selectedFile);
            DBG("Preset saved to: " + selectedFile.getFullPathName());
            
            // Optionally, refresh the preset list in the UI or do other actions
            refreshPresetList();
            applyPresetSelection(selectedFile.getFileNameWithoutExtension());
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
    auto selectedId = presetBox.getSelectedId();
    if (selectedId > 0)
    {
        auto presetName = presetBox.getItemText(selectedId - 1);
        auto presetFile = getPresetFolder().getChildFile(presetName + ".xml");
        audioProcessor.loadPreset(presetFile);
        applyPresetSelection(presetName);
    }
}

void PresetManager::refreshPresetList()
{
    presetBox.clear();
    auto presetFolder = getPresetFolder();
    presetFolder.createDirectory();
    
    auto files = presetFolder.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false);
    
    int index = 1;
    for (auto& file : files)
    {
        presetBox.addItem(file.getFileNameWithoutExtension(), index);
        ++index;
    }
}

juce::File PresetManager::getPresetFolder()
{
    return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("GuitarTools/Presets");
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
