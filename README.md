dwtools
================================================================================
This repository contains some basic tooling to interact with Cray DataWarp.
Many rely on libdatawarp, which must be loaded via `module load datawarp` 
before compilation.

- `dw_rest_explore.py` demonstrates how to use the DataWarp API from within Python
- `dw_stage_file.c` is a benchmark that performs a blocking DataWarp stage-in
  and returns the observed transfer rate
- `dw_get_accounting_data.c` is a wrapper for `dw_get_accounting_data_json(3)`
  API call available in CLE6.0UP04 suitable for inserting in the beginning and
  end of a job script.  Run using `dw_get_accounting_data $DW_JOB_STRIPED` from
  a DataWarp client.
- `dw_get_accounting_data.json` is a sample output from `dw_get_accounting_data.c`
  for a small allocation on Gerty at NERSC

License
--------------------------------------------------------------------------------
This code is not licensed for re-use or redistribution, so please contact me 
(Glenn K. Lockwood) if you would like to use or repurpose anything contained
herein.
