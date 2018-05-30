/*
 * Copyright 2016 <Admobilize>
 * MATRIX Labs  [http://creator.matrix.one]
 * This file is part of MATRIX Creator MALOS
 *
 * MATRIX Creator MALOS is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <algorithm>

#include <matrix_io/malos/v1/driver.pb.h>
#include "./driver_everloop.h"
#include "matrix_hal/everloop_image.h"

namespace pb = matrix_io::malos::v1;

namespace matrix_malos {

bool EverloopDriver::ProcessConfig(const pb::driver::DriverConfig& config) {
  pb::io::EverloopImage image(config.image());

  matrix_hal::EverloopImage image_for_hal(MatrixLeds());
  int idx = 0;
  for (const pb::io::LedValue& value : image.led()) {
    image_for_hal.leds[idx].red = value.red();
    image_for_hal.leds[idx].green = value.green();
    image_for_hal.leds[idx].blue = value.blue();
    image_for_hal.leds[idx].white = value.white();
    if (std::max({value.red(), value.green(), value.blue(), value.white()}) >
        255) {
      zmq_push_error_->Send("ERROR: LED values go from 0 to 255.");
      return false;
    }
    ++idx;
  }

  return writer_->Write(&image_for_hal);
}

bool EverloopDriver::SendUpdate() {
  pb::io::EverloopImage everloop_pb;
  int32_t matrix_leds = MatrixLeds();

  everloop_pb.set_everloop_length(matrix_leds);

  std::string buffer;
  everloop_pb.SerializeToString(&buffer);
  zqm_push_update_->Send(buffer);
  return true;
}
}  // namespace matrix_malos
