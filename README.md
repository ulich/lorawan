# LoRaWAN playground

Playing around with the [edu-kit](http://docs.edu-kit.de/) for the [HanseHack](https://hansehack.de/).

Device code is based on all kind of sketches from http://docs.edu-kit.de/

## Getting started

### device

Copy `device/lora_credentials.example.h` to `device/lora_credentials.h` and put in your credentials you received
from your network provider. `device/lora_credentials.h` is on `.gitignore` so won't be commited unwantedly.

Open the `device/device.ino` file with Arduino IDE and compile and upload the code onto the device.


### UI

Create `ui/.env.local` file with the content
```
REACT_APP_READ_KEY = <your READ_KEY here>
```
`ui/.env.local` is on `.gitignore` so won't be commited unwantedly.

Now inside the `ui` folder run
```
yarn install
yarn start
```
