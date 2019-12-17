################################################################################
#                                                                              #
# Persistent Identifiers management rule set                                   #
#                                                                              #
################################################################################

# Legend (the prefix i stands for iCAT and e for epic):
#
# ePID PID on the EPIC server
# iPID PID record in the iCAT
# eCHECKSUM checksum on the EPIC server
# iCHECKSUM checksum record on the iCAT

# List of the functions:
#
# EUDATCreatePID(*parent_pid, *path, *ror, *iCATCache, *newPID)
# EUDATSearchPID(*path, *existing_pid)
# EUDATSearchPIDchecksum(*path, *existing_pid)
# EUDATUpdatePIDWithNewChild(*parentPID, *childPID)
# EUDATGeteRorPid(*pid, *ror)
# EUDATeiChecksumMgmt(*path, *PID)
# EUDATiPIDcreate(*path, *PID)
# EUDATiFieldVALUEretrieve(*path, *FNAME, *FVALUE)
# EUDATePIDcreate(*path, *extraType, *PID, *ifchecksum)
# EUDATePIDsearch(*field, *value, *PID)
# EUDATeCHECKSUMupdate(*PID, *path)
# EUDATeURLupdate(*PID, *newURL)
# EUDATeURLsearch(*PID, *URL)
# EUDATePIDremove(*path, *force)
# EUDATeiChecksumMgmtColl(*sourceColl)
# EUDATiRORupdate(*source, *pid)
# EUDATeRORupdate(*pid,*newRor)
# EUDATPidsForColl(*collPath)
# 


#
# Generate a new PID for a digital object.
# Fields stored in the PID record: URL, ROR and CHECKSUM
# adds a ROR field if (*ror != "None")
#
# Parameters:
#   *parent_pid [IN]    the PID of the digital object that was replicated to us (not necessarily the ROR)
#   *path       [IN]    the path of the replica to store with the PID record
#   *ror        [IN]    the ROR PID (absolute url) of the digital object that we want to store.
#   *iCATCache  [IN]    the boolean flag to enable creation of PID in the iCAT 
#   *newPID     [OUT]   the pid generated for this replica 
#
# Author: Willem Elbers, MPI-TLA
# Edited by Elena Erastova, RZG; Long Phan, JSC; Robert Verkerk, SURFsara
#
EUDATCreatePID(*parent_pid, *path, *ror, *iCATCache, *newPID) {

    logInfo("create pid for *path and save *ror as ror");
    if (!EUDATisMetadata(*path)) {
        getEpicApiParameters(*credStoreType, *credStorePath, *epicApi, *serverID, *epicDebug);

        #check if PID already exists
        EUDATSearchPID(*path, *existing_pid);
   
        if((*existing_pid == "empty") || (*existing_pid == "None")) {
            # add extraType parameters
            *extraType = "empty";

            # add ror as extratype parameter
            if (*ror != "None" && *ror != "") {
                logInfo("Update Parent PID with field ROR");
                *extraType = "EUDAT/ROR=*ror";
            }        

            # add ppid as extratype parameter
            if (*parent_pid != "None" && *parent_pid != "") {
                logInfo("Update Parent PID with field PPID");
                if (*extraType != "empty") {
                      *extraType = "*extraType"++";EUDAT/PPID=*parent_pid";
                } else {
                      *extraType = "EUDAT/PPID=*parent_pid";
                } 
            }

            # create PID
        
            # Verify the type of the input path (collection / data object)
            msiGetObjType(*path, *type);
        
            # If it is a collection - do not compute checksum
            if (*type == '-c') {
                EUDATePIDcreate(*path, *extraType, *newPID, bool("false"));
            }
            # If it is a data object - compute checksum and add it to PID
            else if (*type == '-d') {
                EUDATePIDcreate(*path, *extraType, *newPID, bool("true"));
            }
            # creation of the file based metadata record
            *checksum = "";
            *modtime = "";
            EUDATStoreJSONMetadata(*path, *newPID, *ror, *checksum, *modtime);

            if (*iCATCache) {
                # Add PID into iCAT
                logInfo("Saving PID into field AVU -PID- of iCAT *path with PID = *newPID");
                EUDATiPIDcreate(*path, *newPID);
            }
 
        }
        else {
            *newPID = *existing_pid;
            logInfo("PID already exists (*newPID)");
        }
    }
    else {
        logInfo("Skipped PID creation on metadata path: *path");
    }
}

# -----------------------------------------------------------------------------
# Searching for a PID using URL replacing "#", "%" and "&" with "*"
# (uses replaceHash in epicclient.py)
# Parameters:
#   *path       	[IN]    the path of the replica
#   *existing_pid	[OUT]   existing PID
#   *status             [REI]   false if no value is found, true elsewhere 
#
# Author: Elena Erastova, RZG
# -----------------------------------------------------------------------------
EUDATSearchPID(*path, *existing_pid) {
    logInfo("search pid for *path");
    getEpicApiParameters(*credStoreType, *credStorePath, *epicApi, *serverID, *epicDebug);
#    msiExecCmd("epicclient.py","*credStoreType *credStorePath replaceHash *path", 
#               "null", "null", "null", *out1);
    EUDATReplaceHash(*path, *path1);
    *status = EUDATePIDsearch("URL", "*serverID"++"*path1", *existing_pid);
    *status;
}

#
# Searching fo a PID using CHECKSUM
# Parameters:
#   *path       	[IN]    the path of the replica
#   *existing_pid	[OUT]   existing PID 
#
# Author: Elena Erastova, RZG
#
EUDATSearchPIDchecksum(*path, *existing_pid) {
    logInfo("search pid for *path");
    getEpicApiParameters(*credStoreType, *credStorePath, *epicApi, *serverID, *epicDebug);

    #check if checksum already exists
    *checksum = "";
    msiSplitPath(*path,*parent,*child);
    EUDATiCHECKSUMretrieve(*path, *checksum, *modtime);
    
    if(*checksum == "") {
        *existing_pid ="empty";
    }
    else {
        EUDATePIDsearch("CHECKSUM", *checksum, *existing_pid);
        msiExecCmd("epicclient.py", "*credStoreType *credStorePath read *existing_pid --key URL",
                   "null", "null", "null", *out);
        msiGetStdoutInExecCmdOut(*out, *URL);
        msiSplitPath(*URL,*parent1,*child1);
        if("*serverID*parent" != *parent1) {
            *existing_pid ="empty";
            logInfo("parent  = *serverID*parent ; parent1 = *parent1");
        }

        logInfo("search by CHECKSUM inside PID registry, got PID = *existing_pid");
    }
}

#
# Update a PID record with a new child.
#
# Parameters:
#       *parentPID  [IN]    PID of the record that will be updated
#       *childPID   [IN]    PID to store as one of the child locations
#
# Author: Willem Elbers, MPI-TLA
# Modified by: Claudio Cacciari, CINECA
#
EUDATUpdatePIDWithNewChild(*parentPID, *childPID) {
    logInfo("update parent pid (*parentPID) with new child (*childPID)");
    getEpicApiParameters(*credStoreType, *credStorePath, *epicApi, *serverID, *epicDebug);
    if(*epicDebug > 1) {
        logDebug("epicclient.py *credStoreType *credStorePath relation *parentPID *epicApi*childPID");
    }
    msiExecCmd("epicclient.py","*credStoreType *credStorePath relation *parentPID *epicApi*childPID",
               "null", "null", "null", *out);
    msiGetStdoutInExecCmdOut(*out, *response);
    if(*epicDebug > 1) {
    	logDebug("update handle location response = *response");
    }
}

#
# get the ROR entry for a PID
#
# Parameters:
#   *pid    [IN]     PID that you want to get the ROR for
#   *ror    [OUT]    ROR for *pid
#
EUDATGeteRorPid(*pid, *ror) {
    logInfo("get RoR from (*pid)");
    getEpicApiParameters(*credStoreType, *credStorePath, *epicApi, *serverID, *epicDebug);
    msiExecCmd("epicclient.py", "*credStoreType *credStorePath read *pid --key EUDAT/ROR", 
               "null", "null", "null", *out);
    msiGetStdoutInExecCmdOut(*out, *ror);
    if(*ror=="None") {
        msiExecCmd("epicclient.py", "*credStoreType *credStorePath read *pid --key ROR", 
                   "null", "null", "null", *out);
        msiGetStdoutInExecCmdOut(*out, *ror);
        if(*ror=="None") {
            logInfo("geteRorPID -> NO ROR for *pid ");
        }
    }
}

#
# This function creates a PID and stores its value and the checksum in the iCAT if it does not exist.
# Otherwhise the function modifies the PID.
#
# Arguments:
#   *path            [IN]    Path of the source file
#   *PID             [OUT]   PID of the source file
#   *iCATCache       [IN]    Specify whether you want to use the iCAT (bool("true")) or not
#
# Author: Giacomo Mariani, CINECA; Claudio Cacciari CINECA;
#
EUDATeiChecksumMgmt(*path, *PID) {

    *PID = "empty";
    logInfo("[EUDATeiChecksumMgmt] Look if the PID is in the iCAT");
    if (!EUDATiFieldVALUEretrieve(*path, "PID", *PID)) {
        EUDATSearchPID(*path, *PID);
    }
    if ( *PID != "empty" ) {
        logInfo("[EUDATeiChecksumMgmt] Update PID *PID with CHECKSUM for obj *path"); 
        EUDATeCHECKSUMupdate(*PID, *path);  
    }
    else {
        logError("[EUDATeiChecksumMgmt] no PID found")
    }  
}

# -----------------------------------------------------------------------------
# The function write iPID given ePID.
#
# Parameters:
#   *PID             [IN] ePID
#   *path            [IN] path of the object
#
# Author: Giacomo Mariani, CINECA
# Modified: Elena Erastova, RZG, 27.08.2015
#------------------------------------------------------------------------------
EUDATiPIDcreate(*path, *PID) {

    *FVALUE = "None";
    *status0 = bool("false");
    EUDATiFieldVALUEretrieve(*path, "PID", *FVALUE);
    if(*FVALUE == "None") { 
        EUDATCreateAVU("PID", *PID, *path);
        *status0 = bool("true");
    }
    else if(*FVALUE != *PID) {
        logError("EUDATiFieldVALUEretrieve (path *path PID *PID) -> "
              ++ "Existing iCAT attribute PID value *FVALUE does not match the input value *PID.");
        *status0 = bool("false");
    }
    else {
        logInfo("EUDATiFieldVALUEretrieve (path *path PID *PID) -> "
             ++ "Attribute PID with value *FVALUE already exists. Nothing to do.");
        *status0 = bool("true");
    }
    *status0;
}

#
# The function retrieves the value of the required field from iCAT.
#
# Arguments:
#   *path               [IN]    the iRODS path of the object involved in the query
#   *FNAME              [IN]    the name of the iCAT field the function is going to look for
#   *FVALUE             [OUT]   the corresponding value, if any
#   *status0            [REI]   false if no value is found, true elsewhere
#
# Author: Giacomo Mariani, CINECA
#
EUDATiFieldVALUEretrieve(*path, *FNAME, *FVALUE) {
    logInfo("EUDATiFieldVALUEretrieve -> looking for *FNAME of *path");
    *status0 = bool("false");
    msiGetObjType(*path,*type);
    if (*type == '-c')  {
        *d = SELECT META_COLL_ATTR_VALUE WHERE COLL_NAME = '*path' AND META_COLL_ATTR_NAME = '*FNAME';
        foreach(*c in *d) {
            *FVALUE = *c.META_COLL_ATTR_VALUE;
            logInfo("EUDATiFieldVALUEretrieve -> *FNAME equal to= *FVALUE");
            *status0 = bool("true");
        }
    }
    else {
        msiSplitPath(*path, *coll, *name);
        *d = SELECT META_DATA_ATTR_VALUE WHERE DATA_NAME = '*name' AND COLL_NAME = '*coll' AND META_DATA_ATTR_NAME = '*FNAME'; 
        foreach(*c in *d) {
            *FVALUE = *c.META_DATA_ATTR_VALUE;
            logInfo("EUDATiFieldVALUEretrieve -> *FNAME equal to= *FVALUE");
            *status0 = bool("true");
        }
    }
    *status0;
}

#
# The function create ePID.
#
# Environment variable used:
#
# Arguments:
#   *path            [IN]   The full iRODS path of the object
#   *extraType       [IN]   extra parameters
#   *PID             [OUT]  The created ePID.
#
# Author: Giacomo Mariani, CINECA
# Edited by:  Robert Verkerk, SURFsara
#
EUDATePIDcreate(*path, *extraType, *PID, *ifcheksum) {
    getEpicApiParameters(*credStoreType, *credStorePath, *epicApi, *serverID, *epicDebug) ;
    
    if (*ifcheksum == bool("true") ) {
        logInfo("EUDATePIDcreate -> Add PID with CHECKSUM to: USER, OBJPATH: $userNameClient, *path");
        EUDATiCHECKSUMget(*path, *checksum, *modtime);
        *extChksum="";
        if (*checksum != "") {
            *extChksum=" --checksum *checksum";
        }
        *execCmd="*credStoreType *credStorePath create '*serverID"++"*path'"++"*extChksum";
    }
    
    else {
        logInfo("EUDATePIDcreate -> Add PID withOUT CHECKSUM to: USER, OBJPATH: $userNameClient, *path");
        *execCmd="*credStoreType *credStorePath create '*serverID"++"*path'"
    }
    
    if (*extraType != "empty") {
         logInfo("EUDATePIDcreate -> Add PID with extratype parameter: *extraType");
         *execCmd="*execCmd"++" --extratype \"*extraType\"";
    }
    msiExecCmd("epicclient.py","*execCmd","null", "null", "null", *out);
    msiGetStdoutInExecCmdOut(*out, *PID);
    logInfo("EUDATePIDcreate -> Created handle is: *PID");
}

#
# The function retrieve ePID searching for a field between URL, CHECKSUM. 
#
# Environment variable used:
#   $objPath
#
# Arguments:
#   *field           [IN]    The eField to look in
#   *value           [IN]    The value to search for
#   *PID             [OUT]   ePID
#   *status0         [REI]   false if no value is found, true elsewhere
#
# Author: Giacomo Mariani, CINECA
#
EUDATePIDsearch(*field, *value, *PID) {
    logInfo("EUDATePIDsearch -> search the PID with *field equal to *value");
    getEpicApiParameters(*credStoreType, *credStorePath, *epicApi, *serverID, *epicDebug);
    *status0 = bool("true");
    msiExecCmd("epicclient.py","*credStoreType *credStorePath search *field *value", "null", "null", "null", *out);
    msiGetStdoutInExecCmdOut(*out, *PID);
    logInfo("EUDATePIDsearch -> search handle response = *PID");
    if ( str(*PID) == "empty" ) { 
        *status0=bool("false"); 
        logInfo("EUDATePIDsearch -> search handle response = no PID");
    }
    *status0;
}


# This function update the checksum field of the PID  
#
# Arguments:
#   *PID                [IN] The PID associated to the object
#   *path               [IN] Object path
#
# Author: Giacomo Mariani, CINECA

EUDATeCHECKSUMupdate(*PID, *path) {
    getEpicApiParameters(*credStoreType, *credStorePath, *epicApi, *serverID, *epicDebug); 
    logDebug("[EUDATeCHECKSUMupdate] modify checksum related to PID *PID");
    EUDATiCHECKSUMget(*path, *checksum, *modtime);
    msiExecCmd("epicclient.py","*credStoreType *credStorePath modify *PID CHECKSUM *checksum",
               "null", "null", "null", *out);
    msiGetStdoutInExecCmdOut(*out, *response);
    logDebug("[EUDATeCHECKSUMupdate] modify handle response = *response");
    *ror = 'None';
    EUDATStoreJSONMetadata(*path, *PID, *ror, *checksum, *modtime);
}


# This function update the URL field of the PID of $objPath    
#
# Arguments:
#   *PID                [IN] The PID associated to $objPath
#   *newURL             [IN] The new URL to be associated to the PID of $objPath
#
# Author: Giacomo Mariani, CINECA
#
EUDATeURLupdate(*PID, *newURL) {
    getEpicApiParameters(*credStoreType, *credStorePath, *epicApi, *serverID, *epicDebug);
    EUDATeURLsearch(*PID, *oldURL);
    logInfo("EUDATeURLupdate -> modify URL in PID *PID");
    msiExecCmd("epicclient.py","*credStoreType *credStorePath modify *PID URL \"*newURL\"",
               "null", "null", "null", *out);
    msiGetStdoutInExecCmdOut(*out, *response);
    logInfo("EUDATeURLupdate -> modify handle response = *response");
    msiExecCmd("epicclient.py","*credStoreType *credStorePath modify *PID 10320/LOC \"*oldURL\" \"*newURL\"",
               "null", "null", "null", *out);
}

#
# This function search the URL field of the PID
#
# Arguments:
#   *PID                [IN] The PID associated to $objPath
#   *newURL             [IN] The new URL to be associated to the PID
#
# Author: Giacomo Mariani, CINECA
#
EUDATeURLsearch(*PID, *URL) {
    getEpicApiParameters(*credStoreType, *credStorePath, *epicApi, *serverID, *epicDebug);
    logInfo("EUDATeURLsearch -> search URL in PID *PID");
    msiExecCmd("epicclient.py","*credStoreType *credStorePath read *PID --key URL ",
               "null", "null", "null", *out);
    msiGetStdoutInExecCmdOut(*out, *URL);
    logInfo("EUDATeURLsearch -> response = *URL");
}


#
# This function remove an ePID... even if its 10320/loc field is not empty!
# To be improved.       
#
# Arguments:
#   *path           [IN]    The path of the object to be removed
#
# Author: Giacomo Mariani, CINECA
#
EUDATePIDremove(*path, *force) {
    getEpicApiParameters(*credStoreType, *credStorePath, *epicApi, *serverID, *epicDebug) 
    logInfo("EUDATePIDremove -> Removing PID associated to: $userNameClient, *path");

    if (EUDATSearchPID(*path, *pid)) {
        msiExecCmd("epicclient.py","*credStoreType *credStorePath read --key 10320/LOC *pid", "null", "null", "null", *out2);
        msiGetStdoutInExecCmdOut(*out2, *loc10320);
        logInfo("EUDATePIDremove -> get 10320/LOC from handle response = *loc10320");
        if (("*loc10320" like "Error*")||("*loc10320" == "")||("*loc10320" like "None*")) {
            logInfo("EUDATePIDremove -> 10320/LOC does not exist or is empty: PID will be deleted");
            msiExecCmd("epicclient.py","*credStoreType *credStorePath delete *pid",
                       "null", "null", "null", *out3);
            msiGetStdoutInExecCmdOut(*out3, *response3);
            logInfo("EUDATePIDremove -> delete handle response = *response3");
            # The PID record could be associated to a replica.
            # The field 10320/LOC of the parent PID record should be updated
        }
        else if (*force == bool("true")){
            logInfo("EUDATePIDremove -> The PID record *pid contains pointers to other DO copies");
            logInfo("EUDATePIDremove -> It will be deleted anyway");
            msiExecCmd("epicclient.py","*credStoreType *credStorePath delete *pid",
                       "null", "null", "null", *out3);
            msiGetStdoutInExecCmdOut(*out3, *response3);
            logInfo("EUDATePIDremove -> delete handle response = *response3");
        }
        else {
            # The PID record contains pointers to other DO copies.
            # What should we do?
            # Maybe all the copies should be deleted together with the master copy.
            logInfo("EUDATePIDremove -> The PID record *pid contains pointers to other DO copies");
            logInfo("EUDATePIDremove -> Nothing will be deleted");
        }
    }
    else {
        logInfo("EUDATePIDremove -> No PID associated to *path found");
    }
} 

# -----------------------------------------------------------------------------
# Walk through the collection. For each object in the collection 
# it creates the object checksum in the iCAT if it does not exist.
# Otherwhise the function modify the PID.
#
# Arguments:
#   *sourceColl      [IN]    Source colection path
#
# Author: Elena Erastova, RZG
# -----------------------------------------------------------------------------
EUDATeiChecksumMgmtColl(*sourceColl) {
    foreach(*Row in SELECT DATA_NAME,COLL_NAME WHERE COLL_NAME like '*sourceColl/%') {
        *objPath = *Row.COLL_NAME ++ '/' ++ *Row.DATA_NAME;
        logInfo("EUDATeiPIDeiChecksumMgmtColl: object *objPath");
	EUDATeiPIDeiChecksumMgmt(*objPath, *pid);
        EUDATiRORupdate(*objPath, *pid);
    }
}

# -----------------------------------------------------------------------------
# Add or modify the ROR field of the PID of the object to iCAT
#
# Arguments:
#   *source            [IN] Object path
#   *pid               [IN] Object pid
#
# Author: Elena Erastova, RZG 27.08.2015
# -----------------------------------------------------------------------------
EUDATiRORupdate(*source, *pid) {
    
    *iRor = 'None';
    EUDATGeteRorPid(*pid, *eRor);
    if(*eRor != 'None') {
        EUDATiFieldVALUEretrieve(*source, "EUDAT/ROR", *iRor);
        if(*iRor != *eRor) {
            logInfo("EUDATiRORupdate -> Adding/modifying iCAT EUDAT/ROR value 
                     *iRor according to EUDAT/ROR value *eRor for object 
                     *source with PID *pid");
            msiGetObjType(*source, *objType);
            *KVString="EUDAT/ROR=*eRor";
            msiString2KeyValPair(*KVString,*KVPair);
            msiSetKeyValuePairsToObj(*KVPair,*source,*objType);
        }
    }
}

#-----------------------------------------------------------------------------
# Add or modify the ROR field of the PID in EPIC system
#
# Arguments:
#   *source            [IN] Object path
#   *pid               [IN] Object pid
#
# Author: Elena Erastova, RZG, 27.08.2015
#-----------------------------------------------------------------------------
EUDATeRORupdate(*pid,*newRor) {

    getEpicApiParameters(*credStoreType, *credStorePath, *epicApi, *serverID, *epicDebug);
    EUDATGeteRorPid(*pid, *oldRor);
    if(*oldRor != *newRor) {
        msiExecCmd("epicclient.py","*credStoreType *credStorePath modify *pid EUDAT/ROR *newRor", 
                   "null", "null", "null", *out);
        msiGetStdoutInExecCmdOut(*out, *response);
        logInfo("EUDATeCHECKSUMupdate -> modify handle response = *response");
    }
}

#-----------------------------------------------------------------------------
# Create PIDs for all collections and objects in the collection recursively
# ROR is assumed to be "None"
#
# Parameters:
# *collPath    [IN] path of the collection
#
# Author: Elena Erastova, RZG
#-----------------------------------------------------------------------------
EUDATPidsForColl(*collPath) {

    logInfo("[EUDATPidsForColl] Creating PIDs for collection *collPath");

    # Verify that source input path is a collection
    msiGetObjType(*collPath, *type);
    if (*type != '-c') {
        logError("Input path *collPath is not a collection");
        fail;
    }
    # Create PIDs for all subcollections in collection recursively
    foreach(*RowC in SELECT COLL_NAME WHERE COLL_NAME like '*collPath%') {
        *subCollPath = *RowC.COLL_NAME;
        EUDATCreatePID("None", *subCollPath, "None", bool("true"), *newPID);
    }
    # Create PIDs for all data objects in collection recursively
    foreach(*Row in SELECT DATA_NAME,COLL_NAME WHERE COLL_NAME like '*collPath%') {
        *objPath = *Row.COLL_NAME ++ '/' ++ *Row.DATA_NAME;
        EUDATCreatePID("None", *objPath, "None", bool("true"), *newPID);
    }
}
