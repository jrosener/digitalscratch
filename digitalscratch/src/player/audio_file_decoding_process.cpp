/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------( audio_file_decoding_process.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2013                                                   */
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

#include "audio_file_decoding_process.h"

Audio_file_decoding_process::Audio_file_decoding_process(Audio_track *in_at)
{
    qDebug() << "Audio_file_decoding_process::Audio_file_decoding_process: create object...";

    if (in_at == NULL)
    {
        qCritical() << "Audio_file_decoding_process::Audio_file_decoding_process: audio track is NULL";
    }
    else
    {
        this->at = in_at;
    }

    this->file = NULL;

    qDebug() << "Audio_file_decoding_process::Audio_file_decoding_process: create object done.";

    return;
}

Audio_file_decoding_process::~Audio_file_decoding_process()
{
    qDebug() << "Audio_file_decoding_process::~Audio_file_decoding_process: delete object...";

    if (this->file != NULL)
    {
        delete this->file;
    }
    qDebug() << "Audio_file_decoding_process::~Audio_file_decoding_process: delete object done.";

    return;
}

bool
Audio_file_decoding_process::run(QString in_path, QString in_music_key)
{
    qDebug() << "Audio_file_decoding_process::run...";

    // Check if path is defined.
    if (in_path == NULL)
    {
        qWarning() << "Audio_file_decoding_process::run: path is NULL.";
        return false;
    }

    // Check if file exists.
    if (this->file != NULL)
    {
        delete this->file;
    }
    this->file = new QFile(in_path);
    if (this->file->exists() == false)
    {
        qWarning() << "Audio_file_decoding_process::run: file does not exists.";
        return false;
    }
    QFileInfo file_info = QFileInfo(*this->file);

    // Decode audio data depending of file extension.
    QString extension = file_info.suffix();

    if (extension == MP3_FILE_EXT)
    {
        this->at->reset();
        if (this->mp3_decode() == false)
        {
            qWarning() << "Audio_file_decoding_process::run: can not decode MP3 audio data.";
            return false;
        }
    }
    else if (extension == FLAC_FILE_EXT)
    {
        this->at->reset();
        if (this->flac_decode() == false)
        {
            cerr << "Audio_file_decoding_process::run: can not decode FLAC audio data.";
            return false;
        }
    }
    /*else if (extension == OGG_FILE_EXT)
    {
        this->at->reset();
        if (this->decode_audio_data_ogg() == FALSE)
        {
            cerr << "Audio_file_decoding_process::run: can not decode OGG audio data.";
            return false;
        }
    }
    */
    else
    {
        qWarning() << "Audio_file_decoding_process::run: unknown extension type.";
        return false;
    }

    // Set name of the track which is for the moment the name of the file.
    this->at->set_name(file_info.fileName());

    // Set file path.
    this->at->set_fullpath(file_info.absoluteFilePath());

    // Set also the music key.
    this->at->set_music_key(in_music_key);

    qDebug() << "Audio_file_decoding_process::run: done.";

    return true;
}

bool
Audio_file_decoding_process::mp3_decode()
{
    qDebug() << "Audio_file_decoding_process::mp3_decode...";

    int               err                   = MPG123_OK;
    mpg123_handle    *handle                = NULL;
    QByteArray        file_name_array       = this->file->fileName().toUtf8();
    char             *file_name             = (char*)file_name_array.constData();
    int               channels              = 0;
    int               encoding              = 0;
    long              rate                  = 0;
    size_t            done                  = 0;
    unsigned int      read_index            = 0;
    unsigned int      max_nb_sample_decoded = 0;
    short signed int *samples               = this->at->get_samples();

    // Initialize mpg123.
    err = mpg123_init();
    if((err != MPG123_OK) ||
       ((handle = mpg123_new(NULL, &err)) == NULL) ||
       (mpg123_open(handle, file_name) != MPG123_OK) ||
       (mpg123_getformat(handle, &rate, &channels, &encoding) != MPG123_OK))
    {
        qWarning() << "Audio_file_decoding_process::mp3_decode: trouble with mpg123: ";
        if (handle == NULL)
            qWarning() << mpg123_plain_strerror(err);
        else
            qWarning() << mpg123_strerror(handle);

        // Cleanup.
        mpg123_close(handle);
        mpg123_delete(handle);
        mpg123_exit();
        return false;
    }

    // Verbosity.
    mpg123_param(handle, MPG123_ADD_FLAGS, MPG123_QUIET, 0.0);

    // Ensure that this output format will not change (it could, when we allow it).
    mpg123_format_none(handle);
    mpg123_format(handle, rate, channels, encoding);

    // Do decoding.
    max_nb_sample_decoded = this->at->get_max_nb_samples() + this->at->get_security_nb_samples();
    do
    {
        // Read and store a piece of data
        err = mpg123_read(handle, (unsigned char*)&(samples[read_index]), max_nb_sample_decoded - read_index, &done);
        read_index += (unsigned int)done / sizeof(short signed int);
        // We are not in feeder mode, so MPG123_OK, MPG123_ERR and
        // MPG123_NEW_FORMAT are the only possibilities.
        // We do not handle a new format, MPG123_DONE is the end... so abort on
        // anything not MPG123_OK.
    } while ((err == MPG123_OK) && (read_index < max_nb_sample_decoded-1));

    // Fail if full decoding was not done.
    if ((err != MPG123_DONE) && (read_index < max_nb_sample_decoded-1))
    {
        qWarning() << "Audio_file_decoding_process::mp3_decode: warning, decoding ended prematurely because: ";
        if (err == MPG123_ERR)
            qWarning() << mpg123_strerror(handle);
        else
            qWarning() << mpg123_plain_strerror(err);
    }

    // Number of samples.
    if (read_index > this->at->get_max_nb_samples())
    {
        this->at->set_end_of_samples(this->at->get_max_nb_samples());
    }
    else
    {
        this->at->set_end_of_samples(read_index);
    }

    // Cleanup.
    mpg123_close(handle);
    mpg123_delete(handle);
    mpg123_exit();

    qDebug() << "Audio_file_decoding_process::mp3_decode: " << this->at->get_end_of_samples()
             << " samples decoded.";

    return true;
}

FLAC__StreamDecoderWriteStatus
flac_write_callback(const FLAC__StreamDecoder *in_decoder,
                    const FLAC__Frame         *in_frame,
                    const FLAC__int32         *const in_buffer[],
                    void                      *in_client_data)
{
    Audio_file_decoding_process* current_instance = static_cast<Audio_file_decoding_process*>(in_client_data);
    return current_instance->flac_write(in_decoder, in_frame, in_buffer);
}

void
flac_metadata_callback(const FLAC__StreamDecoder  *in_decoder,
                       const FLAC__StreamMetadata *in_metadata,
                       void                       *in_client_data)
{
    Audio_file_decoding_process* current_instance = static_cast<Audio_file_decoding_process*>(in_client_data);
    return current_instance->flac_metadata(in_decoder, in_metadata);
}

void
flac_error_callback(const FLAC__StreamDecoder      *in_decoder,
                    FLAC__StreamDecoderErrorStatus  in_status,
                    void                           *in_client_data)
{
    Audio_file_decoding_process* current_instance = static_cast<Audio_file_decoding_process*>(in_client_data);
    return current_instance->flac_error(in_decoder, in_status);
}

bool
Audio_file_decoding_process::flac_decode()
{
    qDebug() << "Audio_file_decoding_process::flac_decode...";

    FLAC__bool                     ok              = true;
    FLAC__StreamDecoder           *decoder         = 0;
    FLAC__StreamDecoderInitStatus  init_status;
    QByteArray                     file_name_array = this->file->fileName().toUtf8();
    char                          *file_name       = (char*)file_name_array.constData();
    this->flac_total_samples = 0;
    this->flac_sample_rate   = 0;
    this->flac_channels      = 0;
    this->flac_bps           = 0;

    if((decoder = FLAC__stream_decoder_new()) == NULL)
    {
        qWarning() << "Audio_file_decoding_process::flac_decode: ERROR: allocating decoder";
        return false;
    }

    (void)FLAC__stream_decoder_set_md5_checking(decoder, true);

    init_status = FLAC__stream_decoder_init_file(decoder, file_name, flac_write_callback,flac_metadata_callback, flac_error_callback, this);
    if(init_status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
    {
        qWarning() << "Audio_file_decoding_process::flac_decode: ERROR: initializing decoder: " << FLAC__StreamDecoderInitStatusString[init_status];
        ok = false;
    }

    if(ok)
    {
        ok = FLAC__stream_decoder_process_until_end_of_stream(decoder);
        if (!ok)
        {
            qWarning() << "Audio_file_decoding_process::flac_decode: decoding FAILED";
        }
        qDebug() << "Audio_file_decoding_process::flac_decode: state: " << FLAC__StreamDecoderStateString[FLAC__stream_decoder_get_state(decoder)];
    }

    FLAC__stream_decoder_delete(decoder);

    qDebug() << "Audio_file_decoding_process::flac_decode: " << this->at->get_end_of_samples()
             << " samples decoded.";

    return true;
}

FLAC__StreamDecoderWriteStatus
Audio_file_decoding_process::flac_write(const FLAC__StreamDecoder *in_decoder,
                                        const FLAC__Frame         *in_frame,
                                        const FLAC__int32         *const in_buffer[])
{
    size_t i;
    char *samples = (char *)this->at->get_samples();

    (void)in_decoder;

    qDebug() << "Audio_file_decoding_process::flac_write...";

    if(this->flac_total_samples == 0)
    {
        qWarning() << "Audio_file_decoding_process::flac_write: ERROR: decoding only works for FLAC files that have a total_samples count in STREAMINFO";
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }
    if(this->flac_channels != 2 || this->flac_bps != 16)
    {
        qWarning() << "Audio_file_decoding_process::flac_write: ERROR: decoding only supports 16bit stereo streams";
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    // Write decoded PCM samples.
    int j = 0;
    if ((this->at->get_end_of_samples() + (in_frame->header.blocksize * 2)) < this->at->get_max_nb_samples())
    {
        for(i = 0; i < in_frame->header.blocksize; i++)
        {
            // Left channel.
            samples[(this->at->get_end_of_samples() + j) * 2]       = (FLAC__uint16)in_buffer[0][i];
            samples[((this->at->get_end_of_samples() + j) * 2) + 1] = ((FLAC__uint16)in_buffer[0][i]) >> 8;
            // Right channel.
            samples[(this->at->get_end_of_samples() + j + 1) * 2]       = (FLAC__uint16)in_buffer[1][i];
            samples[((this->at->get_end_of_samples() + j + 1) * 2) + 1] = ((FLAC__uint16)in_buffer[1][i]) >> 8;

            j = j + 2;
        }
        this->at->set_end_of_samples(this->at->get_end_of_samples() + (in_frame->header.blocksize * 2));
    }
    else
    {
        qDebug() << "Audio_file_decoding_process::flac_write maximum samples decoded";
    }

    qDebug() << "Audio_file_decoding_process::flac_write done.";

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void
Audio_file_decoding_process::flac_metadata(const FLAC__StreamDecoder  *in_decoder,
                                           const FLAC__StreamMetadata *in_metadata)
{
    (void)in_decoder;

    qDebug() << "Audio_file_decoding_process::flac_metadata...";

    // Print some stats.
    if(in_metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
    {
        // Save for later.
        this->flac_total_samples = in_metadata->data.stream_info.total_samples;
        this->flac_sample_rate   = in_metadata->data.stream_info.sample_rate;
        this->flac_channels      = in_metadata->data.stream_info.channels;
        this->flac_bps           = in_metadata->data.stream_info.bits_per_sample;

        qDebug() << "Audio_file_decoding_process::flac_metadata: sample rate    : " << this->flac_sample_rate << " Hz";
        qDebug() << "Audio_file_decoding_process::flac_metadata: channels       : " << this->flac_channels;
        qDebug() << "Audio_file_decoding_process::flac_metadata: bits per sample: " << this->flac_bps;
        qDebug() << "Audio_file_decoding_process::flac_metadata: total samples  : " << this->flac_total_samples;
    }

    qDebug() << "Audio_file_decoding_process::flac_metadata done.";
}

void
Audio_file_decoding_process::flac_error(const FLAC__StreamDecoder      *in_decoder,
                                        FLAC__StreamDecoderErrorStatus  in_status)
{
    (void)in_decoder;

    qDebug() << "Audio_file_decoding_process::flac_error...";

    qWarning() << "Audio_file_decoding_process::flac_error: Got error callback: " << FLAC__StreamDecoderErrorStatusString[in_status];

    qDebug() << "Audio_file_decoding_process::flac_error done.";
}
