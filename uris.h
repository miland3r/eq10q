/***************************************************************************
 *   Copyright (C) 2011 by Pere Ràfols Soler                               *
 *   sapista2@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef EQ10Q_URIS_H
#define EQ10Q_URIS_H

#define EQ_ATOM_URI "http://eq10q.sourceforge.net/atom"
#define EQ_SAMPLE_RATE_KEY EQ_ATOM_URI "#sampleratekey"
#define EQ_SAMPLE_RATE_RESPONSE EQ_ATOM_URI "#samplerate"
#define EQ_FFT_DATA_KEY EQ_ATOM_URI "#fftdatakey"
#define EQ_FFT_DATA_EVENT EQ_ATOM_URI "#fftdataevent"
#define EQ_FFT_ON EQ_ATOM_URI "#ffton"
#define EQ_FFT_OFF EQ_ATOM_URI "#fftoff"
#define EQ_SAMPLE_RATE_REQUEST  EQ_ATOM_URI "#sampleraterequest"

typedef struct
{
  LV2_URID atom_Object;
  LV2_URID atom_Double;
  LV2_URID atom_Sequence;
  LV2_URID atom_Vector;
  LV2_URID atom_URID;
  LV2_URID atom_eventTransfer;
  LV2_URID atom_sample_rate_key;
  LV2_URID atom_sample_rate_response;  
  LV2_URID atom_fft_data_key;
  LV2_URID atom_fft_data_event;
  LV2_URID atom_fft_on;
  LV2_URID atom_fft_off;
  LV2_URID atom_sample_rate_request;
} Eq10qURIs;

static inline void map_eq10q_uris(LV2_URID_Map* map, Eq10qURIs* uris)
{
  uris->atom_Object        = map->map(map->handle, LV2_ATOM__Object);
  uris->atom_Double        = map->map(map->handle, LV2_ATOM__Double);
  uris->atom_Sequence      = map->map(map->handle, LV2_ATOM__Sequence);
  uris->atom_Vector        = map->map(map->handle, LV2_ATOM__Vector);
  uris->atom_URID          = map->map(map->handle, LV2_ATOM__URID);
  uris->atom_eventTransfer = map->map(map->handle, LV2_ATOM__eventTransfer); 
  uris->atom_sample_rate_key =  map->map(map->handle, EQ_SAMPLE_RATE_KEY);
  uris->atom_sample_rate_response   = map->map(map->handle, EQ_SAMPLE_RATE_RESPONSE);
  uris->atom_fft_data_key      = map->map(map->handle, EQ_FFT_DATA_KEY);
  uris->atom_fft_data_event      = map->map(map->handle, EQ_FFT_DATA_EVENT);
  uris->atom_fft_on        = map->map(map->handle, EQ_FFT_ON);
  uris->atom_fft_off       = map->map(map->handle, EQ_FFT_OFF);
  uris->atom_sample_rate_request = map->map(map->handle, EQ_SAMPLE_RATE_REQUEST);
}

#endif
