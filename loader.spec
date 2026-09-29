x64:
	push $OBJECT
		make pic +optimize +gofirst

		run "services.spec"

		mergelib "./libs/libtcg.x64.zip"

        run "pico.spec"
            link "pico"

        # dotNet assembly
        resolve "%ASSEMBLY_PATH"
	    load %ASSEMBLY_PATH
	        preplen
	        link "assembly"

        # dotNet assembly arguments
        pack $ASSEMBLY_ARGS_BYTES "z" %ASSEMBLY_ARGS
        push $ASSEMBLY_ARGS_BYTES
            preplen
            link "assembly_args"

        # pipe name
        pack $PIPE_NAME_BYTES "z" %PIPE_NAME
        push $PIPE_NAME_BYTES
            link "pipe_name"

		export