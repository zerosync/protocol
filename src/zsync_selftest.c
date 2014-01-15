/* =========================================================================
    zsync_selftest - run all self tests

   -------------------------------------------------------------------------
   Copyright (c) 2013 Kevin Sapper, Bernhard Finger
   Copyright other contributors as noted in the AUTHORS file.
   
   This file is part of ZeroSync, see http://zerosync.org.
   
   This is free software; you can redistribute it and/or modify it under
   the terms of the GNU Lesser General Public License as published by the
   Free Software Foundation; either version 3 of the License, or (at your
   option) any later version.
   This software is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTA-
   BILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
   Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public License
   along with this program. If not, see http://www.gnu.org/licenses/.
   =========================================================================
*/

#include "zsync_classes.h"

zsync_agent_t *agent;

void
pass_update (char *sender, zlist_t *fmetadata) 
{
    printf ("[ST] PASS_UPDATE from %s: %"PRId64"\n", sender, zlist_size (fmetadata));
    zlist_t *paths = zlist_new ();
    zlist_append (paths, "abc.txt");
    zlist_append (paths, "def.txt");
    zsync_agent_send_request_files (agent, sender, paths);
}


void 
pass_chunk (byte *chunk, char *path, uint64_t sequence, uint64_t offset)
{
    // save chunk
    printf ("[ST] PASS_CHUNK %s, %"PRId64", %"PRId64, path, sequence, offset);
}

zlist_t *
get_update (uint64_t from_state)
{ 
    zlist_t *filemeta_list = zlist_new ();
    zs_fmetadata_t *fmetadata = zs_fmetadata_new ();
    zs_fmetadata_set_path (fmetadata, "%s", "a.txt");
    zs_fmetadata_set_operation (fmetadata, ZS_FILE_OP_UPD);
    zs_fmetadata_set_size (fmetadata, 0x1533);
    zs_fmetadata_set_timestamp (fmetadata, 0x1dfa533);
    zs_fmetadata_set_checksum (fmetadata, 0x3312AFFDE12);
    zlist_append(filemeta_list, fmetadata);
    zs_fmetadata_t *fmetadata2 = zs_fmetadata_new ();
    zs_fmetadata_set_path (fmetadata2, "%s", "b.txt");
    zs_fmetadata_set_renamed_path (fmetadata2, "%s", "c.txt");
    zs_fmetadata_set_operation (fmetadata2, ZS_FILE_OP_REN);
    zs_fmetadata_set_timestamp (fmetadata2, 0x1dfa544);
    zlist_append(filemeta_list, fmetadata2);

    return filemeta_list;
}

// Gets the current state

byte *
get_chunk (char *path, uint64_t chunk_size, uint64_t offset)
{
    return NULL;
}

uint64_t
get_current_state () 
{
    return 0x66;
}

void test_integrate_components ()
{
    printf ("selftest integration* \n");

    agent = zsync_agent_new ();
    zsync_agent_set_pass_update (agent, pass_update);
    zsync_agent_set_pass_chunk (agent, pass_chunk);
    zsync_agent_set_get_update (agent, get_update);
    zsync_agent_set_get_current_state (agent, get_current_state);
    zsync_agent_set_get_chunk (agent, get_chunk);

    zsync_agent_start (agent);

    zlist_t *files;

    while (zsync_agent_running (agent)) {
        zclock_sleep (250);
        zclock_sleep (10000);
        zsync_agent_stop (agent);
    }
    printf ("STOPPED\n");

    zsync_agent_destroy (&agent);
    
    printf ("OK\n");
}

int 
main (int argc, char *argv [])
{
    printf("Running self tests...\n");
    zs_msg_test();
//    zsync_node_test ();
    zsync_agent_test();
    test_integrate_components ();
    printf("Tests passed OK\n");
}

