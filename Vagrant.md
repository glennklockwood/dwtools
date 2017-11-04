# DataWarp Vagrant

This document describes basic procedures for working with DataWarp without using
a workload manager interface, as would be the case on DataWarp development VMs.

## Initialize command line environment

    $ . /etc/profile
    $ module load dws

## Create a DataWarp pool

    $ dwcli create pool --name my_pool --granularity 16MiB
    $ dwcli update node --name $(dwcli ls nodes) --pool my_pool

This creates a pool called `my_pool` with a 16 MiB allocation granularity, then
assigns all known DataWarp nodes to it.  **Question**: how does DataWarp know
what nodes are available?  Is this preconfigured in the VM environment?

## Create a DataWarp allocation

Actually carving off a slice of the DataWarp pool to use as a DataWarp client
requires the following steps:

    $ dwcli create session \
        --hosts $(uname -n) \
        --owner $(id -u) \
        --creator $(whoami) \
        --token my_session

where

* `--hosts $(uname -n)` specifies the clients to which this session will be
  mounted.  **Question**: how does this work with persistent reservations
  whose clients are determined when its activations are created?  Or are all
  clients usually just allowed to access all sessions at this level, and
  correct client mapping is done upon activation?
* `--owner $(id -u)` specifies the user who owns this session
* `--creator` specifies the user who created this session.  I'm not sure
  what this is for, but it is probably related to the idea that only
  privileged users (e.g., the workload manager) can create DataWarp sessions
  on behalf of unprivileged users.
* `--token my_session` is an arbitrary label for this session.  Slurm uses
  this to store the jobid that created this session; I think it's really
  just a place where an upstream middleware can store whatever information
  it wants to.

Then do the following to verify it exists:

    $ dwstat sessions
    sess state     token creator owner             created expiration nodes 
       1 CA--- my_sessio    1000  1000 2017-11-03T23:45:49      never     0 

Note the session id (`1` above).  Create an instance for this session:

    $ dwcli create instance --label my_instance --capacity 512MiB --session 1 --pool my_pool

where `my_instance` is an arbitrary name for this instance and `--session 1`
refers to the session we created in the previous step.  Use `dwstat instances`
to verify that it was created:

    $ dwstat instances
    $ inst state sess  bytes nodes             created expiration intact       label  public confs 
         1 CA---    1 512MiB     1 2017-11-03T23:54:03      never intact my_instance private     0 

Now create a configuration:

    $ dwcli create configuration \
        --root-permissions 0755 \
        --group $(id -g) \
        --access-mode stripe \
        --type scratch \
        --instance 1

where

* `--root-permissions` is required by the REST API but **not** by the
  `dwcli` command.  This should probably have a reasonable default value
  in the `dwcli` command so that it is truly an optional parameter when
  it is required by the API for some `--access-mode` values.
* `--group $(id -g)` is also required by the REST API but **not** by the
  `dwcli` command.  Should probably have a default value or something.
* `--access-mode stripe` defines whether to make a scratch, cache, or swap
  configuration.  You can also specify an invalid value to let the API kick
  back a list of valid options for this field.  Again, not listed as a
  required argument by the `dwcli`, but it is definitely required by the
  REST API.

Verify that it was created and note the `conf` id:

    $ dwstat configurations
    conf state inst    type activs 
       1 CA---    1 scratch      0 

Then create the activation, which is how the configuration actually gets mounted:

    $ dwcli create activation --mount /mnt/bb --session 1 --configuration 1

There will now be a DataWarp allocation available at `/mnt/bb` with 512 MiB of
capacity, as determined when we created the instance.  You can now create files
there, then see the data appear in `/var/opt/cray/dws/mounts/fragments/<fragmentid>`
where `<fragmentid>` can be gleaned from `dwstat fragments`.

