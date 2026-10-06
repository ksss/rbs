# frozen_string_literal: true

module RBS
  # Prototype: lazily materialized `#location`, following Prism.
  #
  # The parser stores `(start_pos << 32) | length` as an Integer in `@location`
  # and the buffer in `@buffer`. The `RBS::Location` object is created on the
  # first call of `#location`. Nodes built from Ruby pass a `Location` (or nil)
  # as before, and `@buffer` stays nil.
  module LazyLocation
    def location
      loc = @location
      return loc unless loc.is_a?(Integer)

      start_pos = loc >> 32
      @location = Location.new(@buffer, start_pos, start_pos + (loc & 0xFFFFFFFF))
    end
  end
end
