## Alternative flow
SDL is built with EXTERNAL_PROPRIETARY  
**User-consent "NO"**  
**User-consent "YES"**

---

#### App activation during active phone call  

Application HMI Type: Media  
OnEventChanged(PHONE_CALL, isActive=true)  

_**Steps:**_  
HMI -> SDL.ActivateApp(\<appID_of_media_app\>) (or \<appID_of_navigation_app\>, \<appID_of_communication_app\>)  

_**Expected:**_  
SDL -> HMI: SDL.ActivateApp (SUCCESS)  
SDL -> app: OnHMIStatus ("HMILevel: FULL, audioStreamingState: NOT_AUDIBLE")  
_when phone call ends_  
SDL -> HMI: (PHONE_CALL, isActive=false)  
SDL -> app: OnHMIStatus ("HMILevel: FULL, audioStreamingState: AUDIBLE")

---

#### App activation during active embedded audio source  
_**Pre-conditions:**_  
Application HMI Type – **Media app  or Communication**  
App in HMILevel: BACKGROUND and audioStreamingState: NOT_AUDIBLE   
Audio source state is active on HMI  
_**Steps:**_  
HMI -> SDL: OnEventChanged (AUDIO_SOURCE, isActive=false)   
HMI -> SDL: SDL. ActivateApp  
_**Expected:**_  
SDL ->app: OnHMIStatus (HMILevel: FULL, audioStreamingState: AUDIBLE)

_**Pre-conditions:**_  
Application HMI Type – **Navigation app**  
Navigation app in HMILevel:LIMITED and audioStreamingState: AUDIBLE  
Audio source state is active on HMI  
"MixingAudioSupported" = **true** at .ini file [link to .ini file]  
_**Steps:**_  
HMI -> SDL.ActivateApp (\<appID_of_navigation_app\>)  
_**Expected:**_    
SDL ->app: OnHMIStatus (HMILevel: FULL, audioStreamingState: AUDIBLE)

:grey_exclamation: **options**  
plication HMI Type – **Navigation app**  
Navigation app in HMILevel:LIMITED and audioStreamingState: AUDIBLE  
Audio source state is active on HMI  
"MixingAudioSupported" = **false** at .ini file [link to .ini file]  
_**Steps:**_  
HMI -> SDL.ActivateApp (\<appID_of_navigation_app\>)  
_**Expected:**_  
SDL -> HMI: SDL. ActivateApp (SUCCESS)  
SDL -> app: OnHMIStatus (HMILevel: FULL, audioStreamingState: AUDIBLE)

_**Pre-conditions:**_  
Application HMI Type – **Non-media app**  
Non-media app in BACKGROUND and NOT_AUDIBLE due to **active embedded audio source or embedded navigation**  
_**Steps:**_  
HMI -> SDL.ActivateApp(\<appID_of_non-media_app\>)  
_**Expected:**_   
SDL -> HMI: SDL. ActivateApp (SUCCESS)  
SDL -> app: OnHMIStatus (HMILevel: FULL, audioStreamingState: NOT_AUDIBLE)

---

#### App activation during active embedded navigation source
_**Pre-conditions:**_  
Application HMI Type – **Media app or Communication app**  
Media app in HMILevel: LIMITED and audioStreamingState: AUDIBLE  
Navi source state is active on HMI  
_**Steps:**_  
HMI -> SDL: SDL. ActivateApp  
_**Expected:**_  
SDL -> HMI: SDL. ActivateApp (SUCCESS)  
SDL -> app: OnHMIStatus (HMILevel: FULL, audioStreamingState: AUDIBLE)  
:grey_exclamation: _note_ (embedded navigation is still active)

_**Pre-conditions:**_  
Application HMI Type – **Navigational app**  
Navigation app in BACKGROUND and NOT_AUDIBLE  
Navi source state is active on HMI  
_**Steps:**_  
HMI -> SDL: OnEventChanged (EMBEDDED_NAVI, isActive=false)  
HMI -> SDL.ActivateApp (\<appID_of_navigation_app\>)  
_**Expected:**_  
SDL -> HMI: SDL.ActivateApp (SUCCESS)  
SDL -> app: OnHMIStatus (FULL, AUDIBLE) 

_**Pre-conditions:**_  
Application HMI Type – **Non-media app**  
Non-media app in BACKGROUND and NOT_AUDIBLE due to **active embedded audio source or embedded navigation**  
_**Steps:**_  
HMI -> SDL.ActivateApp(\<appID_of_non-media_app\>)  
_**Expected:**_  
SDL -> HMI: SDL. ActivateApp (SUCCESS)  
SDL -> app: OnHMIStatus (HMILevel: FULL, audioStreamingState: NOT_AUDIBLE)

