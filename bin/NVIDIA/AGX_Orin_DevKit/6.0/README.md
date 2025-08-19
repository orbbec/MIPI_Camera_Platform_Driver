# ORBBEC camera driver for GMSL

## Install G335Lg/G345Lg driver to Jetson

Copy gmsl-driver compressed package to the target Jetson and Copy them to the right places:

```
#Extract the archive and cd into the folder

# AGX_Orin + G335Lg + FG96_8CH_GMSL (EVK): if use FG96_8CH_GMSL_V2 Dser board and use PWM01/TSC3 generate multi-camera synchronization signals.
sh copy_to_target_agx_orin_fg96.sh

# AGX_Orin + G335Lg + LI-JAG-ADP-GMSL2-8CH: if use Leopard LI-JAG-ADP-GMSL2-8CH Dser board and use TSC1 generate multi-camera synchronization signals.
sh copy_to_target_agx_orin_leopard.sh

# AGX_Orin + G335Lg/G345Lg + LI-JAG-ADP-GMSL2-8CH: If use LI-JAG-ADP-GMSL2-8CH Dser board, use TSC1 generate multi-camera synchronization signals and place the Metadata data in the first row of the image.
sh copy_to_target_agx_orin_nomtd_leopard.sh

# AGX_Orin + G335Lg + MIC-FG-8G: if use ADVANTECH MIC-FG-8G Dser board and use TSC1 generate multi-camera synchronization signals.
sh copy_to_target_agx_orin_mic_fg_8g.sh

# AGX_Orin + G335Lg/G345Lg + FG96_8CH_GMSL (EVK): If use FG96_8CH_GMSL_V2 Dser board, use TSC1 generate multi-camera synchronization signals and place the Metadata data in the first row of the image.
sh copy_to_target_agx_orin_nomtd_fg96.sh

# Orin_NX +  G335Lg + FG96_2CH: if use FG96_2CH_V2 Dser board and use pwm8 generate multi-camera synchronization signals.
sh copy_to_target_orin_nx_fg96.sh    
    
#Reboot system to reconfigure
sudo reboot

```
