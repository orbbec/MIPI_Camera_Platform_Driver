# ORBBEC camera driver for GMSL

## Install On Target

Copy `gmsl-driver-jetson` to the target Jetson, then run one script matching the
board and deserializer hardware:

```bash
cd gmsl-driver-jetson
```

AGX Orin:

```bash
# AGX Orin + G300 + FG96-8CH GMSL (EVK): use FG96_8CH_GMSL_V2 deserializer board and PWM01/TSC3 to generate multi-camera synchronization signals.
./copy_to_target_agx_orin_fg96.sh

# AGX Orin + G300 + FG96-8CH GMSL (EVK): use FG96_8CH_GMSL_V2 deserializer board, PWM01/TSC3 synchronization, and no metadata row mode.
./copy_to_target_agx_orin_nomtd_fg96.sh

# AGX Orin + G300 + Leopard LI-JAG-ADP-GMSL2-8CH: use TSC0 to generate multi-camera synchronization signals.
./copy_to_target_agx_orin_leopard.sh

# AGX Orin + G300 + Leopard LI-JAG-ADP-GMSL2-8CH: use TSC0 synchronization and no metadata row mode.
./copy_to_target_agx_orin_nomtd_leopard.sh

# AGX Orin + G300 + Advantech MIC-FG-8G: use TSC0 to generate multi-camera synchronization signals.
./copy_to_target_agx_orin_mic_fg_8g.sh

# AGX Orin + G300 + FG12-16CH multi-deserializer board: use TSC0 to generate multi-camera synchronization signals.
./copy_to_target_agx_orin_fg12.sh

# AGX Orin + G300 obcam-thin + FG96-8CH GMSL (EVK): use FG96_8CH_GMSL_V2 deserializer board and PWM01/TSC3 synchronization; SerDes/G300 stream configuration is handled by userspace.
./copy_to_target_agx_orin_fg96_obcam.sh

# AGX Orin + G300 obcam-thin + Leopard LI-JAG-ADP-GMSL2-8CH: use TSC0 synchronization; SerDes/G300 stream configuration is handled by userspace.
./copy_to_target_agx_orin_leopard_obcam.sh

```

Orin NX:

```bash
# Orin NX + G300 + FG96 2CH: use pwm8 to generate multi-camera synchronization signals.
./copy_to_target_orin_nx_fg96.sh

# Orin NX + G300 + FG96 2CH: use pwm8 synchronization and no metadata row mode.
./copy_to_target_orin_nx_nomtd_fg96.sh
```

Thor:

```bash
# Thor(D317) + G300 + D317 GM deserializer board: use pwm9 to generate multi-camera synchronization signals.
./copy_to_target_thor_dm317.sh

# Thor(MIC-742/MIC-743) + G300 + MIC-FG-8G deserializer: use TSC0 to generate multi-camera synchronization signals.
./copy_to_target_thor_mic_fg_8g.sh
```

Each script copies the selected G300 DTBO to `/boot`, installs the required
modules under `/lib/modules/$(uname -r)`, runs `depmod`, and applies the Jetson-IO
hardware configuration.

Reboot after installation.

## Remove

On the target Jetson:

```bash
cd gmsl-driver-jetson
./remove_g300.sh
```

Then reboot.

