
#include "LabSoundIncludes.h"
#include "LiveReverbRecording.h"

using namespace LabSound;
void liveReverbRecording(RefPtr<AudioContext> context, float seconds, char const*const path)
{
    ExceptionCode ec;
    //--------------------------------------------------------------
    // Play live audio through a reverb convolution
    //
    SoundBuffer ir(context, "impulse-responses/tim-warehouse/cardiod-rear-35-10/cardiod-rear-levelled.wav");
    //SoundBuffer ir(context, "impulse-responses/filter-telephone.wav");
    
    RefPtr<MediaStreamAudioSourceNode> input = context->createMediaStreamSource(ec);
    RefPtr<ConvolverNode> convolve = context->createConvolver();
    convolve->setBuffer(ir.audioBuffer.get());
    RefPtr<GainNode> wetGain = context->createGain();
    wetGain->gain()->setValue(2.f);
    RefPtr<GainNode> dryGain = context->createGain();
    dryGain->gain()->setValue(1.f);
    
    input->connect(convolve.get(), 0, 0, ec);
    convolve->connect(wetGain.get(), 0, 0, ec);
    wetGain->connect(context->destination(), 0, 0, ec);
    dryGain->connect(context->destination(), 0, 0, ec);
    
    RefPtr<RecorderNode> recorder = RecorderNode::create(context.get(), 44100);
    recorder->startRecording();
    dryGain->connect(recorder.get(), 0, 0, ec);
    wetGain->connect(recorder.get(), 0, 0, ec);
    
    std::cout << "Starting recording" << std::endl;
    
    while (seconds > 0) {
        seconds -= 0.01f;
        usleep(10000);
    }
    
    std::cout << "Done" << std::endl;
    
    recorder->stopRecording();
    std::vector<float> data;
    recorder->getData(data);
    FILE* f = fopen(path, "wb");
    if (f) {
        fwrite(&data[0], 1, data.size(), f);
        fclose(f);
    }
}
