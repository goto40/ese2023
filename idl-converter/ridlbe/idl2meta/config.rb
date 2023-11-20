#--------------------------------------------------------------------
# config.rb - IDL language mapping configuration for test backend
#
# Author: Martin Corino
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the RIDL LICENSE which is
# included with this program.
#
# Copyright (c) Remedy IT Expertise BV
#
# modified/rewritten code from test example, Licence included
#--------------------------------------------------------------------

module IDL
  module Idl2meta
    COPYRIGHT = "demo code; see LICENSE".freeze
    TITLE = 'demo code to transform idl 2 meta-code'.freeze
    VERSION = {
        major: 0,
        minor: 1,
        release: 0
    }

    ## Configure Test backend
    #
    Backend.configure('idl2meta', File.dirname(__FILE__), TITLE, COPYRIGHT, VERSION) do |becfg|

      # setup backend option handling
      #
      becfg.on_setup do |optlist, ridl_params|
        # defaults
        ridl_params[:expand_includes] = false
      end

      # process input / generate code
      # arguments:
      #   in parser - parser object with full AST from parsed source
      #   in options - initialized option hash
      #
      becfg.on_process_input do |parser, options|
        IDL::Idl2meta.process_input(parser, options)
      end # becfg.on_process_input

    end # Backend.configure

    def self.process_input(parser, options, outstream = nil)
      # has a user defined output filename been set
      fixed_output = !options[:output].nil?

      # generate client stubs if requested
      co = outstream || (unless fixed_output
                            GenFile.new(nil, output_file: $stdout)
                          else
                            GenFile.new(options[:output])
                          end)
      begin
        # process StubWriter
        parser.visit_nodes(::IDL::Idl2MetaStubWriter.new(co, options))
      rescue => e
        IDL.log(0, e)
        IDL.log(0, e.backtrace.join("\n")) unless e.is_a? IDL::ParseError
        exit 1
      end

    end

    module ScannerMixin
      def chk_identifier(ident)
        ident
      end
    end # module ScannerMixin

    IDL::Scanner.class_eval do
      include ScannerMixin
    end
  end # module Ruby
end # module IDL
