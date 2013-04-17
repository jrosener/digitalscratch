/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------( audio_file_decoding_process.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2012                                                   */
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

#ifndef AUDIO_FILE_DECODING_PROCESS_H_
#define AUDIO_FILE_DECODING_PROCESS_H_

#include <iostream>
#include <mpg123.h>
#include <FLAC/stream_decoder.h>
#include <QFileInfo>
#include <QFile>
#include <QString>
#include <audio_track.h>
#include <application_const.h>

using namespace std;

#define MP3_FILE_EXT  "mp3"
#define FLAC_FILE_EXT "flac"
#define OGG_FILE_EXT  "ogg"

class Audio_file_decoding_process
{
 private:
    Audio_track *at;
    QFile       *file;

    // Flac decoding specific.
    FLAC__uint64 flac_total_samples;
    unsigned     flac_sample_rate;
    unsigned     flac_channels;
    unsigned     flac_bps;

 public:
    Audio_file_decoding_process(Audio_track *in_at);
    virtual ~Audio_file_decoding_process();

    bool run(QString in_path);         // Make decoding of the audio file depending of its extension.

 private:   
    bool mp3_decode();                 // Make mp3 decoding and id3 (artist + track name).

    bool flac_decode();                // Decode flac encoded file.

    friend FLAC__StreamDecoderWriteStatus flac_write_callback(const FLAC__StreamDecoder *in_decoder,
                                                              const FLAC__Frame         *in_frame,
                                                              const FLAC__int32         *const in_buffer[],
                                                              void                      *in_client_data);
    FLAC__StreamDecoderWriteStatus flac_write(const FLAC__StreamDecoder *in_decoder,
                                              const FLAC__Frame         *in_frame,
                                              const FLAC__int32         *const in_buffer[]);

    friend void flac_metadata_callback(const FLAC__StreamDecoder  *in_decoder,
                                       const FLAC__StreamMetadata *in_metadata,
                                       void                       *in_client_data);
    void flac_metadata(const FLAC__StreamDecoder  *in_decoder,
                       const FLAC__StreamMetadata *in_metadata);

    friend void flac_error_callback(const FLAC__StreamDecoder      *in_decoder,
                                    FLAC__StreamDecoderErrorStatus  in_status,
                                    void                           *in_client_data);
    void flac_error(const FLAC__StreamDecoder      *in_decoder,
                    FLAC__StreamDecoderErrorStatus  in_status);

};

#endif /* AUDIO_FILE_DECODING_PROCESS_H_ */
