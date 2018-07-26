#ifndef FIRMWARE_H
#define FIRMWARE_H

#include <QString>

#define UPDATE_DAT_SIZE             312348
#define BLOCK_SIZE                  80
#define XOR_TABLE_SIZE              0x100
#define XOR_TABLE_OFFSET            0x1FC00
#define FIRMWARE_SIGNATURE_OFFSET   0x1E3FC
#define FIRMWARE_SIGNATURE          0x5AA5AA55

#define SERIAL_OFFSET               0x1FD00
#define FLASH_SIZE                  0x20000
#define BOOTLOADER_SIZE             0x1800
#define ENCRYPTED_FIRMWARE_SIZE     0x25D00
#define UNENCRYPTED_FIRMWARE_SIZE   0x1E400

#define WRITE_COMMAND               0xAA
#define ERASE_COMMAND               0xCC
#define RESET_COMMAND               0xFF
#define REPORT_COMMAND              0x00

#define DUMPER_READ_FLASH           0x01
#define DUMPER_WRITE_BOOTLOADER     0x02
#define DUMPER_WRITE_CONFIG         0x03
#define DUMPER_WRITE_INFO           0x04
#define DUMPER_INFO                 0x05

#define A_BOOTLOADER_CRC            0x1B8960EF
#define CS_BOOTLOADER_CRC           0xFB3DED05
#define BAD_CRC                     0xC8C2F013

#define TL866_VID 0x04D8
#define TL866_PID 0xE11C


class Firmware
{
public:

    Firmware();
    int open(const QString &filename);
    bool isValid();
    unsigned char GetEraseParammeter(int type);
    unsigned char GetFirmwareVersion();
    void decrypt_firmware(unsigned char *data_out, int type);
    void encrypt_firmware(const unsigned char *data_in, unsigned char *data_out, int key);
    void get_firmware(unsigned char *data_out, int type, int key);
    void encrypt_serial(unsigned char *key, const unsigned char *firmware);
    void decrypt_serial(unsigned char *key, const unsigned char *firmware);
    static bool IsBadCrc(uchar *devcode, uchar *serial);


    typedef struct {
        uchar   echo;
        uchar   device_status;
        ushort  report_size;
        uchar   firmware_version_minor;
        uchar   firmware_version_major;
        uchar   device_version;
        uchar   device_code[8];
        uchar   serial_number[24];
        uchar   hardware_version;
        uchar   b0;
        uchar   b1;
        uchar   b2;
        uchar   b3;
    }TL866_REPORT;


    typedef struct{
        uchar   device_code[8];
        uchar   serial_number[24];
        uchar   bootloader_version;
        uchar   cp_bit;
    }DUMPER_REPORT;


    enum BootloaderType{A_BOOTLOADER, CS_BOOTLOADER};
    enum FirmwareType{FIRMWARE_A, FIRMWARE_CS, FIRMWARE_CUSTOM};
    enum DEVICE_VERSION{VERSION_TL866A = 1, VERSION_TL866CS = 2};
    enum DEVICE_STATUS{NORMAL_MODE = 1, BOOTLOADER_MODE = 2};


    enum
    {
        NoError,
        OpenError,
        FilesizeError,
        CRCError,
        DecryptionError
    };


    enum ENCRYPTION_KEY
    {
        A_KEY  = VERSION_TL866A,
        CS_KEY = VERSION_TL866CS
    };

private:

    typedef struct {
        unsigned char header[4];//file header
        unsigned int  A_CRC32;//4 bytes
        unsigned char pad1;
        unsigned char A_erase;
        unsigned char pad2;
        unsigned char pad3;
        unsigned int  CS_CRC32;//4 bytes
        unsigned char pad4;
        unsigned char CS_erase;
        unsigned char pad5;
        unsigned char pad6;
        unsigned int  A_Index;//index used in A firmware decryption
        unsigned char A_Xortable1[256];//First xortable used in A firmware decryption
        unsigned char A_Xortable2[1024];//Second xortable used in A firmware decryption
        unsigned int  CS_Index;//index used in CS firmware decryption
        unsigned char CS_Xortable1[256];//First xortable used in CS firmware decryption
        unsigned char CS_Xortable2[1024];//Second xortable used in CS firmware decryption
        unsigned char A_Firmware[ENCRYPTED_FIRMWARE_SIZE];//Encrypted A firmware
        unsigned char CS_Firmware[ENCRYPTED_FIRMWARE_SIZE];//Encrypted CS firmware
    }UpdateDat;

    void encrypt_block(unsigned char *data, unsigned char *xortable, int index);
    void decrypt_block(unsigned char *data, unsigned char *xortable, int index);


    unsigned char m_firmwareA[ENCRYPTED_FIRMWARE_SIZE ];
    unsigned char m_firmwareCS[ENCRYPTED_FIRMWARE_SIZE ];
    unsigned char m_eraseA;
    unsigned char m_eraseCS;
    unsigned char m_version;
    bool m_isValid;

    unsigned char m_xortableA[XOR_TABLE_SIZE] = {
        0xA4, 0x1E, 0x42, 0x8C, 0x3C, 0x76, 0x14, 0xC7, 0xB8, 0xB5, 0x81, 0x4A, 0x13, 0x37, 0x7C, 0x0A,
        0xFE, 0x3B, 0x63, 0xC1, 0xD5, 0xFD, 0x8C, 0x39, 0xD1, 0x1F, 0x22, 0xC7, 0x7F, 0x4D, 0x2F, 0x15,
        0x71, 0x21, 0xF9, 0x25, 0x33, 0x44, 0x92, 0x93, 0x80, 0xD7, 0xAB, 0x1B, 0xB6, 0x11, 0xA9, 0x5A,
        0x88, 0x29, 0xFB, 0xD9, 0xF3, 0x76, 0xAA, 0x47, 0x73, 0xD5, 0x31, 0x06, 0x76, 0x4B, 0x90, 0xEA,
        0x11, 0xEB, 0x9C, 0x3D, 0xF2, 0xFA, 0x99, 0x06, 0x96, 0x52, 0x0A, 0x8A, 0xBC, 0x04, 0xC8, 0x14,
        0x19, 0x41, 0x52, 0xF2, 0x4D, 0x7B, 0x64, 0xC0, 0x16, 0xC7, 0xCB, 0xE9, 0xC3, 0x86, 0x77, 0x6A,
        0xEC, 0x44, 0xD2, 0xD9, 0x61, 0xE0, 0x50, 0xA6, 0x60, 0xED, 0x47, 0xA2, 0x0B, 0x59, 0x02, 0xBD,
        0x18, 0x4C, 0x11, 0x14, 0xCB, 0x53, 0xE2, 0x2B, 0x21, 0xBE, 0x96, 0x76, 0x4F, 0x47, 0x0D, 0x1F,
        0x6A, 0xF4, 0x43, 0x03, 0x68, 0x3E, 0xE0, 0xFE, 0x47, 0x72, 0x0A, 0x68, 0x8C, 0x58, 0x7E, 0xDF,
        0xEF, 0x13, 0xDF, 0x47, 0x55, 0x48, 0x4D, 0x10, 0xFE, 0x82, 0x3A, 0xB7, 0x00, 0xD5, 0x79, 0x90,
        0xF4, 0xC2, 0x98, 0xC2, 0xEF, 0x5B, 0x70, 0x93, 0xB4, 0xA7, 0xFA, 0xE6, 0x27, 0x48, 0x65, 0x01,
        0x05, 0x5B, 0x65, 0x94, 0xD3, 0xA0, 0xCD, 0xF7, 0x14, 0xDB, 0x60, 0xB4, 0xBF, 0x7A, 0xE4, 0x45,
        0xF0, 0x77, 0x79, 0x1F, 0xDE, 0x80, 0x29, 0xEF, 0x0D, 0x56, 0xC0, 0x23, 0xC5, 0x73, 0xDE, 0xAC,
        0xC2, 0xEF, 0x4A, 0x02, 0x2D, 0xA4, 0x89, 0x69, 0xCB, 0x91, 0xB0, 0x74, 0x75, 0x7C, 0x76, 0xC7,
        0xC8, 0xDB, 0x8D, 0x20, 0x1D, 0xF5, 0x33, 0x99, 0xBB, 0x45, 0x04, 0x27, 0x4C, 0x1F, 0x12, 0x67,
        0x8E, 0x96, 0x37, 0x9A, 0x4B, 0x9C, 0xAA, 0xED, 0x8B, 0x6B, 0xD1, 0xFF, 0x08, 0x24, 0x56, 0x9D
    };

    unsigned char m_xortableCS[XOR_TABLE_SIZE] = {
        0x0B, 0x08, 0x07, 0x18, 0xEC, 0xC7, 0xDF, 0x8C, 0xD6, 0x76, 0xCE, 0x10, 0x9F, 0x61, 0x7C, 0xF5,
        0x61, 0x09, 0xFB, 0x59, 0xD0, 0x24, 0xB4, 0x4F, 0xCA, 0xE4, 0xA1, 0x3A, 0x30, 0x7C, 0xBD, 0x7A,
        0xF5, 0xE1, 0xB9, 0x4B, 0x74, 0xCD, 0xF1, 0xE9, 0x07, 0x0A, 0x9E, 0xF9, 0xD5, 0xED, 0x4D, 0x24,
        0xEB, 0x21, 0x90, 0x05, 0x8F, 0xA5, 0xF3, 0x45, 0xD0, 0x18, 0x31, 0x04, 0x62, 0x35, 0xA8, 0x7B,
        0xA9, 0x9A, 0x0B, 0xE0, 0x14, 0xCD, 0x57, 0x8A, 0xAC, 0x80, 0x08, 0x56, 0xED, 0x14, 0x8C, 0x49,
        0xD4, 0x5D, 0xF8, 0x77, 0x39, 0xA5, 0xFA, 0x23, 0x5F, 0xF3, 0x0E, 0x27, 0xCA, 0x8D, 0xF5, 0x97,
        0x50, 0xBB, 0x64, 0xA1, 0x73, 0xCE, 0xF9, 0xB7, 0xEE, 0x61, 0x72, 0xF1, 0x8E, 0xDF, 0x21, 0xAC,
        0x43, 0x45, 0x9B, 0x78, 0x77, 0x29, 0xB1, 0x31, 0x9E, 0xFC, 0xA1, 0x6B, 0x0F, 0x8C, 0x8D, 0x13,
        0x12, 0xCC, 0x2B, 0x54, 0x3A, 0xD8, 0xBF, 0xB8, 0xF5, 0x34, 0x46, 0x90, 0x61, 0x54, 0xF4, 0x95,
        0x61, 0x62, 0xE1, 0xCF, 0xF1, 0x3B, 0x00, 0xB6, 0xB6, 0xBB, 0x50, 0x98, 0xD9, 0x3A, 0x56, 0x3A,
        0x16, 0x56, 0xCA, 0xC2, 0x10, 0xF3, 0x91, 0xD4, 0xE8, 0x81, 0xEB, 0xFC, 0x0D, 0x7E, 0xEE, 0x4C,
        0x56, 0x3B, 0x33, 0x46, 0x4E, 0xE2, 0xCF, 0xFC, 0xCF, 0xB8, 0x84, 0x75, 0xD2, 0xA0, 0x39, 0x53,
        0x85, 0xE1, 0xA8, 0xB3, 0x9E, 0x28, 0x57, 0x55, 0xEF, 0xD1, 0xC9, 0xFD, 0x3B, 0x62, 0xF5, 0x18,
        0x49, 0x58, 0xF7, 0xA3, 0x36, 0x27, 0x06, 0x49, 0x0F, 0x7C, 0xA6, 0xCB, 0xA0, 0xC5, 0x1E, 0xA5,
        0x86, 0xF3, 0x2D, 0xEF, 0x8C, 0x7E, 0xF9, 0x81, 0x34, 0xAA, 0x48, 0x5A, 0x93, 0x0A, 0xF2, 0x43,
        0x62, 0x42, 0x97, 0xAF, 0x53, 0x10, 0x8D, 0xE6, 0xA1, 0x8E, 0x1C, 0x62, 0xEB, 0xB1, 0xEE, 0x79
    };

};

#endif // FIRMWARE_H
