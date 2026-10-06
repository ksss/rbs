# frozen_string_literal: true

module RBS
  module AST
    module Directives
      class Base
      end

      class Use < Base
        prepend TableLocation

        class SingleClause
          prepend TableLocation

          attr_reader :type_name, :new_name, :location

          def initialize(type_name:, new_name:, location:, buffer: nil)
            @type_name = type_name
            @new_name = new_name
            @location = location
            @buffer = buffer
          end
        end

        class WildcardClause
          prepend TableLocation

          attr_reader :namespace, :location

          def initialize(namespace:, location:, buffer: nil)
            @location = location
            @buffer = buffer
            @namespace = namespace
          end
        end

        attr_reader :clauses, :location

        def initialize(clauses:, location:, buffer: nil)
          @clauses = clauses
          @location = location
          @buffer = buffer
        end
      end

      class ResolveTypeNames < Base
        attr_reader :location

        attr_reader :value

        def initialize(value:, location:)
          @value = value
          @location = location
        end
      end
    end
  end
end
