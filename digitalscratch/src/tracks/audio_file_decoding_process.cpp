/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------( audio_file_decoding_process.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2020                                                   */
/*                Julien Rosener <julien.rosener@digital-scratch.org>         */
/*                                                                            */
/*----------------------------------------------------------------( License )-*/
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This package is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program. If not, see <http://www.gnu.org/licenses/>.      */
/*                                                                            */
/*------------------------------------------------------------( Description )-*/
/*                                                                            */
/*    Behavior class: process to decode an audio file (mp3, flac, ogg,...)    */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>
#include <algorithm>

#include <samplerate.h>
extern "C"
{
    #include "libavutil/log.h"
    #include "libswresample/swresample.h"
    #include "libavutil/opt.h"
}

#include "app/application_logging.h"
#include "tracks/audio_file_decoding_process.h"

Audio_file_decoding_process::Audio_file_decoding_process(const QSharedPointer<Audio_track> &at,
                                                         const bool &do_resample)
{
    if (at.data() == nullptr)
    {
        qCCritical(DS_FILE) << "audio track is null";
    }
    else
    {
        this->at = at;
        this->do_resample = do_resample;
        this->decoded_sample_rate = this->at->get_sample_rate();

        // Init libAV log level.
        av_log_set_level(AV_LOG_QUIET);
    }

    return;
}

Audio_file_decoding_process::~Audio_file_decoding_process()
{
    return;
}

void
Audio_file_decoding_process::clear()
{
    this->at->reset();
}

bool
Audio_file_decoding_process::run(const QString &path,
                                 const QString &file_hash,
                                 const QString &music_key)
{
    // Check if file exists.
    this->file.setFileName(path);
    if (this->file.exists() == false)
    {
        qCWarning(DS_FILE) << "file" << path << "does not exists";
        return false;
    }

    // Decode compressed audio.
    this->at->reset();
    if (this->decode() == false)
    {
        qCWarning(DS_FILE) << "can not decode" << path;
        return false;
    }

    // Set name of the track which is for the moment the name of the file.
    QFileInfo file_info = QFileInfo(this->file);
    this->at->set_name(file_info.fileName());
    if (this->at->get_name() == "")
    {
        emit name_changed("--");
    }
    else
    {
        emit name_changed("[" + this->at->get_length_str() + "]  " + this->at->get_name());
    }

    // Set file path.
    this->at->set_fullpath(file_info.absoluteFilePath());

    // Set also the music key.
    this->at->set_music_key(music_key);
    emit key_changed(this->at->get_music_key());

    // Set file hash.
    this->at->set_hash(file_hash);

    return true;
}

void
Audio_file_decoding_process::resample_track()
{
    if ((this->do_resample == true) && (at->get_sample_rate() != this->decoded_sample_rate))
    {
        // Copy decoded samples in a temp buffer.
        unsigned int input_nb_samples = at->get_end_of_samples();
        float *input_samples = new float[input_nb_samples];
        src_short_to_float_array(at->get_samples(), input_samples, input_nb_samples);

        // Resample temp buffer.
        int   output_nb_samples = (at->get_end_of_samples() * (float)at->get_sample_rate() / (float)this->decoded_sample_rate) + 2;
        float *output_samples = new float[output_nb_samples];
        SRC_DATA src_data;
        src_data.data_in       = input_samples;
        src_data.data_out      = output_samples;
        src_data.end_of_input  = 0;
        src_data.input_frames  = input_nb_samples / 2;
        src_data.output_frames = output_nb_samples / 2;
        src_data.src_ratio     = (float)at->get_sample_rate() / (float)this->decoded_sample_rate;
        src_simple(&src_data, SRC_LINEAR, 2);

        // Copy resampled sampler back to original table of samples.
        src_float_to_short_array(output_samples, at->get_samples(), src_data.output_frames_gen * 2);
        at->set_end_of_samples(src_data.output_frames_gen * 2);

        // Cleanup.
        delete [] input_samples;
        delete [] output_samples;
    }

    return;
}

int
Audio_file_decoding_process::decode_packet_to_frame(AVCodecContext* codec_context,
                                                    AVFrame* frame,
                                                    int &got_frame,
                                                    AVPacket *packet)
{
    int ret;

    got_frame = 0;
    ret = avcodec_send_packet(codec_context, packet);
    if (ret < 0)
    {
        // In particular, we don't expect AVERROR(EAGAIN), because we read all
        // decoded frames with avcodec_receive_frame() until done.
        if (ret == AVERROR_EOF)
            return 0;
        else
            return ret;
    }

    ret = avcodec_receive_frame(codec_context, frame);
    if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
        return ret;
    if (ret >= 0)
        got_frame = 1;

    return 0;
}

bool
Audio_file_decoding_process::decode()
{
    // Get file name to decode.
    QByteArray        filename_array = this->file.fileName().toUtf8();
    char             *filename       = (char*)filename_array.constData();

    // Get output table of decoded samples.
    short signed int *output_samples = this->at->get_samples();

    // Allocate a frame.
    AVFrame* frame = av_frame_alloc();
    if (!frame)
    {
        return false;
    }

    // Open file.
    AVFormatContext* format_context = nullptr;
    if (avformat_open_input(&format_context, filename, nullptr, nullptr) != 0)
    {
        av_free(frame);
        qCWarning(DS_FILE) << "error opening file" << qPrintable(filename);
        return false;
    }

    // Get audio format.
    if (avformat_find_stream_info(format_context, nullptr) < 0)
    {
        av_free(frame);
        avformat_close_input(&format_context);
        qCWarning(DS_FILE) << "error finding the stream info" << qPrintable(filename);
        return false;
    }

    // Find the audio stream (some container files can have multiple streams in them).
    AVStream* audio_stream = nullptr;
    for (unsigned int i = 0; i < format_context->nb_streams; ++i)
    {
        if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audio_stream = format_context->streams[i];
            break;
        }
    }
    if (audio_stream == nullptr)
    {
        av_free(frame);
        avformat_close_input(&format_context);
        qCWarning(DS_FILE) << "could not find any audio stream in the file" << qPrintable(filename);
        return false;
    }

    // Get decoder for the codec of the audio file.
    AVCodec *codec = avcodec_find_decoder(audio_stream->codecpar->codec_id);
    AVCodecContext* codec_context = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_context, audio_stream->codecpar);
    if (codec_context->codec == nullptr)
    {
        av_free(frame);
        avformat_close_input(&format_context);
        qCWarning(DS_FILE) << "couldn't find a proper decoder" << qPrintable(filename);
        return false;
    }
    else if (avcodec_open2(codec_context, codec_context->codec, nullptr) != 0)
    {
        av_free(frame);
        avformat_close_input(&format_context);
        qCWarning(DS_FILE) << "couldn't open the context with the decoder" << qPrintable(filename);
        return false;
    }

    // Store decoded sample rate.
    this->decoded_sample_rate = codec_context->sample_rate;

    // Show audio format.
    qCInfo(DS_FILE) << qPrintable(this->file.fileName()) << ":"
                    << codec_context->sample_rate << "Hz,"
                    << codec_context->channels << "ch,"
                    << av_get_sample_fmt_name(codec_context->sample_fmt);

    // Set up SWR (software resample) context for a "float planar" to "int interleaved" conversion.
    SwrContext *swr = nullptr;
    if (codec_context->sample_fmt == AV_SAMPLE_FMT_FLTP)
    {
        swr = swr_alloc();
        av_opt_set_int(swr, "in_channel_layout",  codec_context->channel_layout, 0);
        av_opt_set_int(swr, "out_channel_layout", codec_context->channel_layout,  0);
        av_opt_set_int(swr, "in_sample_rate",     codec_context->sample_rate, 0);
        av_opt_set_int(swr, "out_sample_rate",    codec_context->sample_rate, 0);
        av_opt_set_sample_fmt(swr, "in_sample_fmt",  AV_SAMPLE_FMT_FLTP, 0);
        av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16,  0);
        swr_init(swr);
    }

    // Create a packet.
    AVPacket packet;
    av_init_packet(&packet);

    // Read the packets in a loop
    bool decoding_done = false;
    int got_frame = 0;
    while ((decoding_done == false) && (av_read_frame(format_context, &packet) == 0))
    {
        if (packet.stream_index == audio_stream->index)
        {
            got_frame = 0;
            if (this->decode_packet_to_frame(codec_context, frame, got_frame, &packet) != 0)
            {
                qCWarning(DS_FILE) << "packet decode error " << qPrintable(filename);
            }
            else if (got_frame == 1)
            {
                // Frame now has usable audio data in it.
                if (codec_context->sample_fmt == AV_SAMPLE_FMT_S16) // Interleaved data.
                {
                    int total_nb_samples = frame->nb_samples * codec_context->channels;
                    if ((this->at->get_end_of_samples() + total_nb_samples) > this->at->get_max_nb_samples())
                    {
                        // We reached the end of the audio track buffer.
                        total_nb_samples = this->at->get_max_nb_samples() - this->at->get_end_of_samples();
                        decoding_done = true;
                    }
                    int data_size = total_nb_samples * sizeof(short signed int);
                    memcpy(output_samples, frame->data[0], data_size);
                    output_samples += total_nb_samples;
                    this->at->set_end_of_samples(this->at->get_end_of_samples() + total_nb_samples);
                }
                else if (codec_context->sample_fmt == AV_SAMPLE_FMT_S16P) // Planar data (one data table per channels).
                {
                    int total_nb_samples = frame->nb_samples * codec_context->channels;
                    if ((this->at->get_end_of_samples() + total_nb_samples) > this->at->get_max_nb_samples())
                    {
                        // We reached the end of the audio track buffer.
                        total_nb_samples = this->at->get_max_nb_samples() - this->at->get_end_of_samples();
                        decoding_done = true;
                    }
                    short signed int *channel_0 = (short signed int *)frame->data[0];
                    short signed int *channel_1 = (short signed int *)frame->data[1];
                    for (int i = 0; i < frame->nb_samples; i++)
                    {
                        if (channel_0 != nullptr)
                        {
                            output_samples[i*2] = channel_0[i];
                        }
                        else
                        {
                            output_samples[i*2] = 0;
                        }
                        if (channel_1 != nullptr)
                        {
                            output_samples[i*2+1] = channel_1[i];
                        }
                        else
                        {
                            output_samples[i*2+1] = 0;
                        }
                    }
                    output_samples += total_nb_samples;
                    this->at->set_end_of_samples(this->at->get_end_of_samples() + total_nb_samples);
                }
                else if (codec_context->sample_fmt == AV_SAMPLE_FMT_FLTP) // Float (-1.0 to 1.0) planar data (one data table per channels).
                {
                    int total_nb_samples = frame->nb_samples * codec_context->channels;
                    if ((this->at->get_end_of_samples() + total_nb_samples) > this->at->get_max_nb_samples())
                    {
                        // We reached the end of the audio track buffer.
                        total_nb_samples = this->at->get_max_nb_samples() - this->at->get_end_of_samples();
                        decoding_done = true;
                    }
                    int data_size = total_nb_samples * sizeof(short signed int);

                    uint8_t* frame_s16;
                    av_samples_alloc(&frame_s16, NULL, codec_context->channels, frame->nb_samples * codec_context->channels, AV_SAMPLE_FMT_FLTP, 0);
                    swr_convert(swr,
                                &frame_s16, frame->nb_samples * codec_context->channels, // out buffer
                                (const uint8_t **)frame->extended_data, frame->nb_samples); // in buffer
                    memcpy(output_samples, frame_s16, data_size);
                    av_freep(&frame_s16);
                    output_samples += total_nb_samples;
                    this->at->set_end_of_samples(this->at->get_end_of_samples() + total_nb_samples);
                }
                else // Non recognized byte format.
                {
                    decoding_done = true;
                    qCWarning(DS_FILE) << "audio byte format not supported" << qPrintable(filename);
                }
            }
        }

        // Cleanup packet.
        av_packet_unref(&packet);
    }

    // Cleanup.
    av_free(frame);
    avcodec_free_context(&codec_context);
    avformat_close_input(&format_context);
    if (swr != nullptr)
    {
        swr_free(&swr);
    }

    // Maybe the sample rate used by the sound card to play the file is not the same as
    // the one of the audio file, so convert it if necessary.
    this->resample_track();

    return true;
}
