# frozen_string_literal: true

module RBS
  # Prototype: lazily materialized `#location`, following Prism.
  #
  # Nodes built by the parser keep an Integer in `@location` and the buffer in
  # `@buffer`; the `RBS::Location` object is created on the first call of
  # `#location`. Nodes built from Ruby pass a `Location` (or nil) as before.
  #
  # `__raw_location` / `__location_buffer` let code that rebuilds a node of the
  # *same class* pass the location through without materializing it.
  module LazyLocationPassThrough
    def __raw_location = @location
    def __location_buffer = @buffer
  end

  # For nodes without child locations: `@location` is `(start_pos << 32) | length`.
  module LazyLocation
    include LazyLocationPassThrough

    def location
      loc = @location
      return loc unless loc.is_a?(Integer)

      start_pos = loc >> 32
      @location = Location.new(@buffer, start_pos, start_pos + (loc & 0xFFFFFFFF))
    end
  end

  # For nodes with child locations: `@location` is an index into the location
  # table the parser attached to `@buffer` (range + child ranges, names from a
  # static per-node-type schema).
  module TableLocation
    include LazyLocationPassThrough

    def location
      loc = @location
      return loc unless loc.is_a?(Integer)

      @location = LocationTable.__materialize(@buffer, loc)
    end
  end
end
