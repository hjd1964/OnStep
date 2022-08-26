const char productVersion[] = "server_version|" Version "\n";

void statusAjax()
{
    String data = "";
    char temp[400] = "";
    char temp1[120] = "";
    char temp2[120] = "";

    Ser.setTimeout(webTimeout);
    serialRecvFlush();

    mountStatus.update(true);

    data += FPSTR(productVersion);

    if (mountStatus.getVer(temp1))
    {
        data += "mount_version|";
        data += temp1;
        data += "\n";
    }

    if (!mountStatus.valid())
    {
        data += "type|invalid\n";
        server.send(200, "text/plain", data);
        return;
    }

    data += "type|valid\n";

    if (mountStatus.lastError()!=ERR_NONE) {
        mountStatus.getLastErrorMessage(temp2);
        sprintf(temp, "last_error|%s\n", temp2);
        data += temp;
    } else {
        data += "last_error|0\n";
    }

    /**
     * Fork type
     */
    if (mountStatus.mountType() == MT_ALTAZM)
    {
        data += "mount_type|alt_az\n";
    }
    else if (mountStatus.mountType() == MT_GEM)
    {
        data += "mount_type|gem\n";

        if (mountStatus.autoMeridianFlips())
        {
            data += "auto_meridian|1\n";
        }
        else
        {
            data += "auto_meridian|0\n";
        }

        if (mountStatus.pauseAtHome())
        {
            data += "pause_at_home|1\n";
        }
        else
        {
            data += "pause_at_home|0\n";
        }
    }
    else if (mountStatus.mountType() == MT_UNKNOWN)
    {
        data += "mount_type|?\n";
    }
    else if (mountStatus.mountType() == MT_FORK)
    {
        data += "mount_type|fork\n";
    }
    else if (mountStatus.mountType() == MT_FORKALT)
    {
        data += "mount_type|fork_alt\n";
    }

    if (mountStatus.mountType() != MT_ALTAZM)
    {
        if (mountStatus.rateCompensation() == RC_NONE)
        {
            data += "rate_comp|none\n";
        }
        else if (mountStatus.rateCompensation() == RC_REFR_RA)
        {
            data += "rate_comp|refr_ra\n";
        }
        else if (mountStatus.rateCompensation() == RC_REFR_BOTH)
        {
            data += "rate_comp|refr_both\n";
        }
        else if (mountStatus.rateCompensation() == RC_FULL_RA)
        {
            data += "rate_comp|full_ra\n";
        }
        else if (mountStatus.rateCompensation() == RC_FULL_BOTH)
        {
            data += "rate_comp|full_both\n";
        }
    }

    /**
     * Buzzer
     */
    if (mountStatus.buzzerEnabled())
    {
        data += "buzzer|1\n";
    }
    else
    {
        data += "buzzer|0\n";
    }

    // RA,Dec current
    if (!command(":GR#", temp1)) strcpy(temp1, "?");
    if (!command(":GD#", temp2)) strcpy(temp2, "?");

    sprintf(temp, "current_ra|%s\n", temp1);
    data += temp;
    sprintf(temp, "current_dec|%s\n", temp2);
    data += temp;

    // RA,Dec target
    if (!command(":Gr#", temp1)) strcpy(temp1, "?");
    if (!command(":Gd#", temp2)) strcpy(temp2, "?");

    sprintf(temp, "target_ra|%s\n", temp1);
    data += temp;
    sprintf(temp, "target_dec|%s\n", temp2);
    data += temp;

    // Site
    if (!command(":GgH#", temp1)) strcpy(temp1, "?");
    temp1[10] = 0;
    if (!command(":GtH#", temp2)) strcpy(temp2, "?");
    temp2[9] = 0;

    sprintf(temp, "long|%s\n", temp1);
    data += temp;
    sprintf(temp, "lat|%s\n", temp2);
    data += temp;

    if (mountStatus.parkFail())
    {
        data += "park_fail|1\n";
    }

    if (mountStatus.atHome())
    {
        data += "home|1\n";
    }
    else
    {
        data += "home|0\n";
    }

    if (mountStatus.parked())
    {
        data += "parked|1\n";
    }
    else
    {
        data += "parked|0\n";
    }

    if (mountStatus.slewing())
    {
        data += "slewing|1\n";
    }
    else
    {
        data += "slewing|0\n";
    }

    if (mountStatus.tracking())
    {
        data += "tracking|1\n";
    }
    else
    {
        data += "tracking|0\n";
    }

    if (mountStatus.aligning())
    {
        data += "aligning|1\n";
    }
    else
    {
        data += "aligning|0\n";
    }

    if (mountStatus.waitingHome()) {
        data += "waiting_at_home|1\n";
    } else {
        data += "waiting_at_home|0\n";
    }

    // gets the current alignment setup
    // Note: this command is undocumented. Docblock taken from
    // lib/Command.ino
    // :A?#
    // where 
    //    m is the maximum number of alignment stars
    //    n is the current alignment star (0 otherwise)
    //    o is the last required alignment star when an alignment is in progress (0 otherwise)
    // aka 913
    command(":A?#", temp1);
    sprintf(temp, "align_stars|%s\n", temp1);
    data += temp;


    // divide current/nominal to get the max slewing rate
    command(":GX92#", temp1);
    sprintf(temp, "nominal_rate|%s\n", temp1);
    data += temp;

    command(":GX93#", temp1);
    sprintf(temp, "current_rate|%s\n", temp1);
    data += temp;

    // GX80 and GX81 are undocumented.
    if (!command(":GX81#", temp1)) strcpy(temp1, "?");
    stripNum(temp1);
    sprintf(temp, "utc_date|%s\n", temp1);
    data += temp;

    // // GX80 is undocumented
    if (!command(":GX80#", temp1)) strcpy(temp1, "?");
    stripNum(temp1);
    sprintf(temp, "utc_time|%s\n", temp1);
    data += temp;
    
    if (!command(":GG#", temp1)) {
        data += "utc_offset|\n";
    } else {
        sprintf(temp, "utc_offset|%s\n", temp1);
        data += temp;
    }

    if (!command(":GS#", temp1)) strcpy(temp1, "?");
    sprintf(temp, "sidereal_time|%s\n", temp1);
    data += temp;

    if (!command(":Gl#", temp1)) strcpy(temp1, "?");
    sprintf(temp, "local_time|%s\n", temp1);
    data += temp;

    byte pierSide = mountStatus.pierSide();

    switch(pierSide) {
        case PierSideNone: data += "pier_side|none\n"; break;
        case PierSideEast: data += "pier_side|east\n"; break;
        case PierSideWest: data += "pier_side|west\n"; break;
        case PierSideBest: data += "pier_side|best\n"; break;
        case PierSideFlipWE1: data += "pier_side|we1\n"; break;
        case PierSideFlipWE2: data += "pier_side|we2\n"; break;
        case PierSideFlipWE3: data += "pier_side|we3\n"; break;
        case PierSideFlipEW1: data += "pier_side|ew1\n"; break;
        case PierSideFlipEW2: data += "pier_side|ew2\n"; break;
        case PierSideFlipEW3: data += "pier_side|ew3\n"; break;
    }

    if ((command(":GT#",temp1)) && (strlen(temp1)>6)) 
    {
        double tr=atof(temp1);
        dtostrf(tr,5,3,temp1);
        sprintf(temp,"tracking_rate|%s\n",temp1);
        data += temp;
    }

    if ((command(":GX97#",temp1)) && (strlen(temp1)>2)) 
    {
        sprintf(temp, "slew_rate|%s\n", temp1);
        data += temp;
    } else {
        // fall back to MaxRate display if not supported
        if ((command(":GX92#", temp1)) && (command(":GX93#", temp2))) 
        { 
            long maxRate=strtol(&temp1[0], NULL, 10);
            long MaxRate=strtol(&temp2[0], NULL, 10);
            sprintf(temp, "slew_rate|%s\n", maxRate);
        } 
        else 
        {
            data += "slew_rate|?";
        }
        data += temp;
    }

    // configuration data
    if (!command(":Gh#",temp1)) strcpy(temp1,"0"); int minAlt=(int)strtol(&temp1[0],NULL,10);
    sprintf(temp,"min_alt|%d\n", minAlt);
    data += temp;

    if (!command(":Go#",temp1)) strcpy(temp1,"0"); int maxAlt=(int)strtol(&temp1[0],NULL,10);
    sprintf(temp,"max_alt|%d\n", maxAlt);
    data += temp;
    
    if (!command(":%BR#",temp1)) strcpy(temp1,"0"); int backlashAxis1=(int)strtol(&temp1[0],NULL,10);
    sprintf_P(temp,"backlash_ra|%d\n",backlashAxis1);
    data += temp;

    if (!command(":%BD#",temp1)) strcpy(temp1,"0"); int backlashAxis2=(int)strtol(&temp1[0],NULL,10);
    sprintf(temp,"backlash_dec|%d\n",backlashAxis2);
    data += temp;
    
 
    if (mountStatus.mountType() == MT_GEM && (command(":GXE9#",temp1)) && (command(":GXEA#",temp2))) {
        int degPastMerE=(int)strtol(&temp1[0],NULL,10);
        degPastMerE=round((degPastMerE*15.0)/60.0);
        sprintf(temp,"deg_past_mer_e|%d\n",degPastMerE);
        data += temp;

        int degPastMerW=(int)strtol(&temp2[0],NULL,10);
        degPastMerW=round((degPastMerW*15.0)/60.0);
        sprintf(temp,"deg_past_mer_w|%d\n",degPastMerW);
        data += temp;
    }

    server.send(200, "text/plain", data);
}

void ajaxRunCommand() 
{
    String cmd = server.arg("cmd");
    char result[40] = "";
    char cmdBuffer[40] = "";

    if (cmd != "") {
        cmd.toCharArray(cmdBuffer, 40);
        processCommand(cmdBuffer, result, cmdTimeout);
        server.send(200, "text/plain", result);
    }
}

void ajaxLibrary() {
    int cat = server.arg("cat").toInt();
    char temp[40]="";
    String data = "";
    bool success = false;

    sprintf(temp,":Lo%ld#",(long)cat-1);

    if (commandBool(temp)) {
        while (true) {
            command(":LR#",temp);

            if (temp[0] == ',') { success=true; break; }
            if (temp[0] == 0)  break;
            data += temp;
            data += "\n";
        }
    }

    server.send(200, "text/plain", data);
}