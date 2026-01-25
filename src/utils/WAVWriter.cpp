#include "WAVWriter.h"

#include <utility>

WAVWriter::WAVWriter() {
}

WAVWriter::~WAVWriter() {

}

bool WAVWriter::begin() {
    return sd_manager.begin();
}

void WAVWriter::end() {
    sd_manager.closeFile(&file);
    //sd_manager.end();
}

void WAVWriter::setSampleRate(int sampleRate) {
    info.setSampleRate(sampleRate);
}

void WAVWriter::setChannels(int channels) {
    info.setNumChannels(channels);
}

void WAVWriter::setName(String name) {
    sd_manager.closeFile(&file);
    _name = std::move(name);
}

bool WAVWriter::writeHeader() {
    file = sd_manager.openFile(_name, true);

    if (!file) return false;

    file.seekSet(0);

    file.write((byte*)&info,sizeof(info));

    sd_manager.closeFile(&file);
    return true;
}

// bool WAVWriter::writeHeaderSizes() {
//     sd_manager.write_block_at(&file, 40, (byte*)&info.subchunk2Size,4);

//     info.chunkSize = 36 + info.subchunk2Size;
//     sd_manager.write_block_at(&file, 4, (byte*)&info.chunkSize,4);

//     return true;
// }

bool WAVWriter::writeHeaderSizes() {
    // 确保文件是打开的
    if (!file.isOpen()) {
        file = sd_manager.openFile(_name, true);
        if (!file.isOpen()) return false;
    }
    
    sd_manager.write_block_at(&file, 40, (byte*)&info.subchunk2Size, 4);

    info.chunkSize = 36 + info.subchunk2Size;
    sd_manager.write_block_at(&file, 4, (byte*)&info.chunkSize, 4);

    return true;
}

// bool WAVWriter::writeChunk(uint8_t *block, int size) {
//     unsigned int written = sd_manager.write_block(&file, block, size);
//     info.subchunk2Size += written;
//     return written == size;
// }

bool WAVWriter::writeChunk(uint8_t *block, int size) {
    // 确保文件是打开的
    if (!file.isOpen()) {
        Serial.println("ERROR: File not open in writeChunk");
        return false;
    }
    
    unsigned int written = sd_manager.write_block(&file, block, size);
    if (written != size) {
        Serial.print("WARNING: Written ");
        Serial.print(written);
        Serial.print(" bytes, expected ");
        Serial.println(size);
    }
    info.subchunk2Size += written;
    return written == size;
}

bool WAVWriter::startRecording() {
    file = sd_manager.openFile(_name, true);

    info.subchunk2Size = 0;
    info.chunkSize = 36;
    
    // return file;
    return file.isOpen();
}

bool WAVWriter::endRecording() {
    writeHeaderSizes();
    sd_manager.closeFile(&file);
    return true;
}

void WAVWriter::cleanFile() {
    sd_manager.remove(_name);
}
