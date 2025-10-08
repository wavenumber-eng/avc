# avc

This repository has source code for use with FRDM-MCXN947 and the FRDM-AVC shield.

The current project uses the MCUXpresso IDE 

# 1 Build Tooling / Installation

## A - Install MCUXPresso 25.6

MCUXpresso is used to compile the code

https://www.nxp.com/design/design-center/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-integrated-development-environment-ide:MCUXpresso-IDE

<img width="1459" height="895" alt="image" src="https://github.com/user-attachments/assets/9d01a824-0c80-4f60-8aaa-8c75d9a6ea1c" />

<img width="1525" height="564" alt="image" src="https://github.com/user-attachments/assets/ecff09ee-9258-4509-b66d-6084d930b79e" />

## B - Install Segger Ozone V338  (Or the latest version)

We use Segger Ozone for flashing the MCXN947 and debug

https://www.segger.com/products/development-tools/ozone-j-link-debugger/

<img width="2023" height="705" alt="image" src="https://github.com/user-attachments/assets/a4a8d28b-8518-40eb-8f88-f648009a0af2" />


# 2 Building the Starter Project

The starter project is located in **src/avc/avc_core**

<img width="1134" height="738" alt="image" src="https://github.com/user-attachments/assets/8a6c15cb-3d05-4fbb-95c0-f6978d66cd22" />

## A Start MCUXpresso

<img width="1134" height="738" alt="image" src="https://github.com/user-attachments/assets/4e7865b8-b981-4ff3-b827-e3157d6aa2e3" />

You can make a new workspace **avc** (or you can leave the default)

<img width="891" height="470" alt="image" src="https://github.com/user-attachments/assets/1bfd6189-c7a3-4b44-aac5-1e145a13395f" />


## B Install the MCXN947 SDK

When you 1st install, you may need to install the SDK for the MCXN947.  

Search on *MCXN947* and select the **frdmmcxn947**

![sdk_install](https://github.com/user-attachments/assets/d34c41ca-29d5-4d4e-9772-81e568718eef)

<img width="1902" height="1180" alt="image" src="https://github.com/user-attachments/assets/7ff9bbf6-95fa-48ab-a4df-97c249accfb9" />

## C Import the avc_core0 project

After you download/clone this repository,  you need to import the avc_core0 project:

![import](https://github.com/user-attachments/assets/0771909c-a20b-4c25-9d27-2d0dab00ed6d)


# Flashing / Debug

We are using the FRDM-MCXN947 which has a built in debugger.   The debug circuity has been flashed with Segger J-Link firmware.

There are two USB ports on the FRDM-MCXN947:  Connect via this one

<img width="1772" height="689" alt="image" src="https://github.com/user-attachments/assets/8567c978-2b9e-4901-a3cb-e183004bdb03" />

There is a Segger Ozone project  in the folder **src/avc/avc_core0/ozone__core0.jdebug**

Once the USB-C is connected you can open the .jdebug file and flash:

<img width="1111" height="406" alt="image" src="https://github.com/user-attachments/assets/ac449010-4107-4206-beb9-9637ab66a6cf" />

![import](https://github.com/user-attachments/assets/1b235744-7848-40bb-9d6a-ab5e3c17bd96)


