/*!********************************************************************

  Audacity: A Digital Audio Editor

  @file AudioUnitEffect.h

  Dominic Mazzoni
  Leland Lucius

**********************************************************************/
#ifndef AUDACITY_AUDIOUNIT_EFFECT_H
#define AUDACITY_AUDIOUNIT_EFFECT_H



#if USE_AUDIO_UNITS

#include "AudioUnitWrapper.h"

#include "MemoryX.h"
#include <functional>
#include <type_traits>
#include <vector>

#include <AudioToolbox/AudioUnitUtilities.h>
#include <AudioUnit/AudioUnitProperties.h>

#include "../StatefulPerTrackEffect.h"
#include "PluginInterface.h"

#include <wx/weakref.h>

#define AUDIOUNITEFFECTS_VERSION wxT("1.0.0.0")
/* i18n-hint: the name of an Apple audio software protocol */
#define AUDIOUNITEFFECTS_FAMILY EffectFamilySymbol{ wxT("AudioUnit"), XO("Audio Unit") }
class AudioUnitEffect;

class AUControl;

class AudioUnitEffect final
   : public StatefulPerTrackEffect
   , AudioUnitWrapper
{
public:
   using Parameters = PackedArray::Ptr<const AudioUnitParameterID>;

   AudioUnitEffect(const PluginPath & path,
      const wxString & name, AudioComponent component,
      Parameters *pParameters = nullptr,
      AudioUnitEffect *master = nullptr);
   virtual ~AudioUnitEffect();

   // ComponentInterface implementation

   PluginPath GetPath() const override;
   ComponentInterfaceSymbol GetSymbol() const override;
   VendorSymbol GetVendor() const override;
   wxString GetVersion() const override;
   TranslatableString GetDescription() const override;

   // EffectDefinitionInterface implementation

   EffectType GetType() const override;
   EffectFamilySymbol GetFamily() const override;
   bool IsInteractive() const override;
   bool IsDefault() const override;
   RealtimeSince RealtimeSupport() const override;
   bool SupportsAutomation() const override;

   EffectSettings MakeSettings() const override;

   bool SaveSettings(
      const EffectSettings &settings, CommandParameters & parms) const override;
   bool LoadSettings(
      const CommandParameters & parms, EffectSettings &settings) const override;

   bool LoadUserPreset(
      const RegistryPath & name, EffectSettings &settings) const override;
   bool SaveUserPreset(
      const RegistryPath & name, const EffectSettings &settings) const override;

   RegistryPaths GetFactoryPresets() const override;
   bool LoadFactoryPreset(int id, EffectSettings &settings) const override;

   unsigned GetAudioInCount() const override;
   unsigned GetAudioOutCount() const override;

   int GetMidiInCount() const override;
   int GetMidiOutCount() const override;

   size_t SetBlockSize(size_t maxBlockSize) override;
   size_t GetBlockSize() const override;

   int ShowClientInterface(
      wxWindow &parent, wxDialog &dialog, bool forceModal) override;

   bool InitializePlugin();
   bool FullyInitializePlugin();

   // EffectUIClientInterface implementation

   std::shared_ptr<EffectInstance> MakeInstance() const override;
   std::unique_ptr<EffectUIValidator> PopulateUI(
      ShuttleGui &S, EffectInstance &instance, EffectSettingsAccess &access)
   override;
   bool IsGraphicalUI() override;
   bool CloseUI() override;

   bool CanExportPresets() override;
   void ExportPresets(const EffectSettings &settings) const override;
   void ImportPresets(EffectSettings &settings) override;

   bool HasOptions() override;
   void ShowOptions() override;

   // AudioUnitEffect implementation
   
private:
   TranslatableString Export(
      const AudioUnitEffectSettings &settings, const wxString & path) const;
   TranslatableString Import(
      AudioUnitEffectSettings &settings, const wxString & path) const;
   /*!
    @param path only for formatting error messages
    @return error message
    */
   TranslatableString SaveBlobToConfig(const RegistryPath &group,
      const wxString &path, const void *blob, size_t len,
      bool allowEmpty = true) const;

   void GetChannelCounts();

   bool MigrateOldConfigFile(
      const RegistryPath & group, EffectSettings &settings) const;
   bool LoadPreset(const RegistryPath & group, EffectSettings &settings) const;
   bool SavePreset(const RegistryPath & group,
      const AudioUnitEffectSettings &settings) const;

#if defined(HAVE_AUDIOUNIT_BASIC_SUPPORT)
   bool CreatePlain(wxWindow *parent);
#endif

private:
   AudioUnitEffectSettings mSettings;

public:
   //! This function will be rewritten when the effect is really stateless
   AudioUnitEffectSettings &GetSettings(EffectSettings &) const
      { return const_cast<AudioUnitEffect*>(this)->mSettings; }
   //! This function will be rewritten when the effect is really stateless
   const AudioUnitEffectSettings &GetSettings(const EffectSettings &) const
      { return mSettings; }

private:
   const PluginPath mPath;
   const wxString mName;
   const wxString mVendor;

   // Initialized in GetChannelCounts()
   unsigned mAudioIns{ 2 };
   unsigned mAudioOuts{ 2 };

   bool mInteractive{ false };
   UInt32 mBlockSize{ 0 };

   bool mUseLatency{ true };

   wxWindow *mParent{};
   wxString mUIType; // NOT translated, "Full", "Generic", or "Basic"
   bool mIsGraphical{ false };
   mutable bool mInitialFetchDone{ false };
};

#endif

#endif
