function Decoder(bytes, port) {
  return {
    frame_counter: bytes[0]
  }
}