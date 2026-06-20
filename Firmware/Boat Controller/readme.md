# Boat Controller — reference duplicate ⚠️

This folder is a **copy** of the boat firmware, kept next to the transmitter project
for convenient cross-referencing while working on the transmitter.

**The canonical boat firmware lives at `Firmware/Boat_Controller/`.** Edit and build
from there; this copy can drift and should ideally be deleted.

## Docs

- **Boat firmware overview, build, BOM, safety, changelog:**
  [`../../Boat_Controller/readme.md`](../../Boat_Controller/readme.md)
- **Deep radio/packet contract:** [`CONTROLLER_HANDOVER.md`](CONTROLLER_HANDOVER.md)
- **Transmitter setup, code recipes, pairing:** [`../TRANSMITTER_GUIDE.md`](../TRANSMITTER_GUIDE.md)

> If you keep this copy, remember to mirror any boat-firmware change into
> `Firmware/Boat_Controller/` (and vice-versa) — they are currently in sync.
