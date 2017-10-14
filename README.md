dwtools
================================================================================
This repository contains some basic tooling to interact with Cray DataWarp.
Many rely on libdatawarp, which must be loaded via `module load datawarp` 
before compilation.  To compile, you would do something like

    cc -o dw_stage_file dw_stage_file.c `pkg-config --cflags --libs cray-datawarp`

A Makefile is also included for reference.

DataWarp C API Examples
--------------------------------------------------------------------------------

- `dw_stage_file.c` is a benchmark that performs a blocking DataWarp stage-in
  and returns the observed transfer rate
- `dw_get_accounting_data.c` is a wrapper for `dw_get_accounting_data_json(3)`
  API call available in CLE6.0UP04 suitable for inserting in the beginning and
  end of a job script.  Run using `dw_get_accounting_data $DW_JOB_STRIPED` from
  a DataWarp client.

DataWarp REST API Examples
--------------------------------------------------------------------------------

- `dw_rest_explore.py` demonstrates how to use the DataWarp API from within
  Python and facilitates interactive exploration of the REST API.  Start by
  running this command with no arguments, then optionally specify paths to see
  what they return.  Not as nice as `dwstat`, but simpler to understand.

Example Output Formats
--------------------------------------------------------------------------------

- `dw_get_accounting_data.json` is a sample output from `dw_get_accounting_data.c`
  for a small allocation on Gerty at NERSC

License
--------------------------------------------------------------------------------
This code is not licensed for re-use or redistribution outside of the U.S.
Department of Energy, so please contact me (Glenn K. Lockwood) if you would like
to use or repurpose anything contained herein.
