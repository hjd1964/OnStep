// -----------------------------------------------------------------------------------
// Library

int currentCatalog = 0;
char currentCatName[12] = "";
bool catalogIndexChanged = false;
bool downloadCatalogData = false;
bool uploadCatalogData = false;
String showMessage = "";
      
const char html_libScript1[] PROGMEM =
"<script>\n"
"function s(key,v) {\n"
  "var xhttp = new XMLHttpRequest();\n"
  "var res = 'libraryA.txt?'+key+'='+encodeURIComponent(v)+'&x='+new Date().getTime();\n"
  "xhttp.open('GET', res, true);\n"
  "xhttp.send();\n"
"}\n"
"function t(key,v) {"
  "if (v.length==0) v='DELETE';"
  "s(key,v.replace(/ /g, '_'));"
"}\n"
"function busy() {\n"
  "document.getElementById('message').innerHTML='Working...';"
  "document.getElementById('up').disabled=true;"
  "document.getElementById('down').disabled=true;"
"}\n"
"</script>\n";

const char html_libStart[] PROGMEM =
"<form method='get' action='/lib.htm'>";

const char html_libCatalogSelect1[] PROGMEM =
L_CATALOGS " (<span id='libFree'>?";
const char html_libCatalogSelect2[] PROGMEM =
"</span> " L_RECS_AVAIL ":)<br /><br />Number <select name='selBox' onchange=\"busy(); s('ci',this.value);\">"
"<option value='0' %s>-</option>"
"<option value='1' %s>1</option>"
"<option value='2' %s>2</option>"
"<option value='3' %s>3</option>"
"<option value='4' %s>4</option>"
"<option value='5' %s>5</option>"
"<option value='6' %s>6</option>"
"<option value='7' %s>7</option>"
"<option value='8' %s>8</option>"
"<option value='9' %s>9</option>"
"<option value='10' %s>10</option>"
"<option value='11' %s>11</option>"
"<option value='12' %s>12</option>"
"<option value='13' %s>13</option>"
"<option value='14' %s>14</option>"
"<option value='15' %s>15</option>"
"</select>&nbsp;\n";

const char html_libSubmitCatalog[] PROGMEM =
"<button id='up' type='button' onclick=\"busy(); t('cu',document.getElementById('catData').value);\" disabled>" L_UPLOAD "</button> "
"<button id='down' type='button' onclick=\"busy(); s('cd',1);\" disabled>" L_DOWNLOAD "</button>&nbsp;&nbsp;&nbsp;"
"<button id='clr_btn' type='button' onclick=\"busy(); if (confirm('" L_ARE_YOU_SURE "?')) s('cc','clear')\">" L_CAT_CLEAR_LIB "</button><br /><br />\n";

const char html_libShowMessage[] PROGMEM =
"<div id='message' style='background-color: #222222; color: #aaaaaa; border: 1px solid #551111; width: 400px; padding: 2px;'>" L_NO_CATALOG ".</div><br />\n";

const char html_libEditCatalog[] PROGMEM =
L_CAT_DATA "<br />"
"<textarea id='catData' style='background-color: #222222; color: #ffffff; border: 1px solid #551111;' "
"rows='10' cols='40' oninput=\"document.getElementById('up').disabled=false;\"></textarea><br /><br />\n";

const char html_libCatalogForm[] PROGMEM =
"Data format:<br /><pre>"
"  Object Name Cat    RA       Dec   <br />"
"  ccccccccccc,ccc,HH:MM:SS,sDD*MM:SS<br /></pre>"
"Sample data:<br /><pre>"
"<p style='color:white'>"
"  $PlaNeb1<br />"
"  M97        ,PN ,11:15:53,+54*55:24<br />"
"  M27        ,PN ,20:00:26,+22*46:29<br />"
"</p></pre>"
L_CAT_EXAMPLE1 L_CAT_EXAMPLE2 L_CAT_EXAMPLE3 L_CAT_EXAMPLE4 L_CAT_EXAMPLE5
"    UNK = Unknown<br />"
"    OC  = Open Cluster<br />"
"    GC  = Globular Cluster<br />"
"    PN  = Planetary Nebula<br />"
"    DN  = Dark Nebula<br />"
"    SG  = Spiral Galaxy<br />"
"    EG  = Elliptical Galaxy<br />"
"    IG  = Irregular Galaxy<br />"
"    KNT = Knot<br />"
"    SNR = Supernova Remnant<br />"
"    GAL = Galaxy<br />"
"    CN  = Cluster w/Nebula<br />"
"    STR = Star<br />"
"    PLA = Planet<br />"
"    CMT = Comet<br />"
"    AST = Asteroid<br /><br />"
L_CAT_EXAMPLE6 L_CAT_EXAMPLE7
"<br /></pre>\n";

const char html_libEnd[] PROGMEM =
"</form>\r\n";

#ifdef OETHS
void handleLibrary(EthernetClient *client) {
#else
void handleLibrary() {
#endif
  currentCatalog=0;
  Ser.setTimeout(webTimeout);
  serialRecvFlush();
  
  mountStatus.update();

  char temp[400]="";
  char temp1[120]="";
  
  processLibraryGet();

  sendHtmlStart();

  // send a standard http response header
  String data=FPSTR(html_headB);
  data += FPSTR(html_main_cssB);
  data += FPSTR(html_main_css1);
  data += FPSTR(html_main_css2);
  data += FPSTR(html_main_css3);
  data += FPSTR(html_main_css4);
  sendHtml(data);
  data += FPSTR(html_main_css5);
  data += FPSTR(html_main_css6);
  data += FPSTR(html_main_css7);
  data += FPSTR(html_main_css8);
  data += FPSTR(html_main_cssE);
  data += FPSTR(html_headE);
  data += FPSTR(html_bodyB);
  sendHtml(data);

  // finish the standard http response header
  data += FPSTR(html_onstep_header1); data += "OnStep";
  data += FPSTR(html_onstep_header2);
  if (mountStatus.getVer(temp1)) data += temp1; else data += "?";
  data += FPSTR(html_onstep_header3);
  data += FPSTR(html_linksStatN);
  data += FPSTR(html_linksCtrlN);
  if (mountStatus.featureFound()) data += FPSTR(html_linksAuxN);
  data += FPSTR(html_linksLibS);
#if ENCODERS == ON
  data += FPSTR(html_linksEncN);
#endif
  sendHtml(data);
  data += FPSTR(html_linksPecN);
  data += FPSTR(html_linksSetN);
  data += FPSTR(html_linksCfgN);
#ifndef OETHS
  data += FPSTR(html_linksWifiN);
#endif
  data += FPSTR(html_onstep_header4);
  sendHtml(data);

  // Scripts
  data += FPSTR(html_libScript1);

  // active ajax page is: settingsAjax();
  data +="<script>var ajaxPage='library.txt';</script>\n";
  data +=FPSTR(html_ajax_active);
  data +="<script>auto2Rate=2;</script>";
 
  // OnStep wasn't found, show warning and info.
  if (!mountStatus.valid()) { data+= FPSTR(html_bad_comms_message); sendHtml(data); sendHtmlDone(data); return; }

  data += FPSTR(html_libCatalogSelect1);
  data += FPSTR(html_libCatalogSelect2);
  data += FPSTR(html_libSubmitCatalog);
  data += FPSTR(html_libShowMessage);
  data += FPSTR(html_libEditCatalog);
  data += FPSTR(html_libCatalogForm);

  strcpy(temp,"</div></body></html>");
  data += temp;

  sendHtml(data);
  sendHtmlDone(data);
}

#ifdef OETHS
void libraryAjaxGet(EthernetClient *client) {
#else
void libraryAjaxGet() {
#endif
  processLibraryGet();
#ifdef OETHS
  client->print("");
#else
  server.send(200, "text/html","");
#endif
}

#ifdef OETHS
void libraryAjax(EthernetClient *client) {
#else
void libraryAjax() {
#endif
  String data="";
  data += "libFree|"; data += commandString(":L?#"); data+="\n";

  if (showMessage != "") {
    data += "message|" + showMessage + "\n";
    showMessage="";
  }
  
  if (catalogIndexChanged) {
    if (currentCatalog == 0) data += "clr_btn|" L_CAT_CLEAR_LIB "\n"; else data += "clr_btn|" L_CAT_CLEAR "\n";
    data += "up|disabled\n";
    strcpy(currentCatName,"");
    if (currentCatalog != 0) {
      if (commandBool(":L$#")) {
        char temp[40]="";
        command(":LR#",temp);
        if (temp[0] == '$') {
          for (int i=1; i<10; i++) {
            currentCatName[i-1]=0; currentCatName[i]=0;
            if (temp[i] == ',' || temp[i] == 0 || temp[i] == '#') break;
            currentCatName[i-1]=temp[i];
          }
          if (currentCatName[0] == 0) {
            showMessage = F(L_CAT_NO_NAME);
          } else {
            data += "down|enabled\n";
            showMessage = L_CATALOG " "; showMessage += currentCatName; showMessage += " " L_SELECTED ".";
          }
        } else { data += "down|disabled\n"; showMessage = F(L_CAT_NO_NAME); }
      } else { data += "down|disabled\n"; showMessage = F(L_CAT_GET_NAME_FAIL); }
    } else { data += "down|disabled\n"; showMessage = F(L_CAT_NO_CAT); }
    data += "catData&\n";
    catalogIndexChanged=false;
  } else

  if (downloadCatalogData) {
    if (currentCatalog != 0) {
      data += "catData&";
      bool success=false;
      char temp[40]="";
      sprintf(temp,":Lo%ld#",(long)currentCatalog-1);
      if (commandBool(temp)) {
        data+="$";
        data+=currentCatName;
        data+="\r";

        while (true) {
          command(":LR#",temp);
          if (temp[0] == ',') { success=true; break; }
          if (temp[0] == 0)  break;

          // isolate the individual fields (and error check)
          int i=0;
          String r=temp;
          String nameS="";
          String catS="";
          String raS="";
          String decS="";

          r=r+",";
          i=r.indexOf(","); if (i>=0) { nameS = r.substring(0,i); r=r.substring(i+1); } else { success=false; break; }
          i=r.indexOf(","); if (i>=0) { catS  = r.substring(0,i); r=r.substring(i+1); } else { success=false; break; }
          i=r.indexOf(","); if (i>=0) { raS   = r.substring(0,i); r=r.substring(i+1); } else { success=false; break; }
          i=r.indexOf(","); if (i>=0) { decS  = r.substring(0,i); r=r.substring(i+1); } else { success=false; break; }

          // pad to the correct length (and error check)
          i=11-nameS.length(); if (i>=0) { for (int j=0; j<i; j++) nameS=nameS+" "; } else { success=false; break; }
          i=3-catS.length(); if (i>=0) { for (int j=0; j<i; j++) catS=catS+" "; } else { success=false; break; }

          r=nameS+","+catS+","+raS+","+decS;

          data+=r;
          data+="\r";
        }
        if (!success) showMessage = F(L_CAT_DOWNLOAD_FAIL); else showMessage = F(L_CAT_DOWNLOAD_SUCCESS);
      } else showMessage = F(L_CAT_DOWNLOAD_INDEX_FAIL);
    } else showMessage = F(L_CAT_DOWNLOAD_INDEX_FAIL);
    data+="\n";

    downloadCatalogData=false;
  }

#ifdef OETHS
  client->print(data);
#else
  server.send(200, "text/plain",data);
#endif
}

void processLibraryGet() {
  String v;
  int i;
  char temp[40]="";

  // Catalog clear
  v=server.arg("cc");
  if (v!="") {
    if (currentCatalog >= 0 && currentCatalog < 16) {
      if (currentCatalog == 0) {
        // clear library
        commandBool(":Lo0#");
        commandBlind(":L!#");
      } else {
        // clear this catalog
        sprintf(temp,":Lo%ld#",(long)currentCatalog-1);
        commandBool(temp);
        commandBlind(":LL#");
      }
      catalogIndexChanged=true;
      showMessage=L_CAT_DATA_REMOVED ".";
    }
  }

  // Catalog download
  v=server.arg("cd");
  if (v!="") {
    if (currentCatalog > 0 && currentCatalog < 16) {
      sprintf(temp,":Lo%ld#",(long)currentCatalog-1);
      if (!commandBool(temp)) currentCatalog=0;
      downloadCatalogData=true;
    }
  }

  // Catalog upload
  // Object Name|Cat|---RA---|---Dec---
  // ccccccccccc,ccc,HH:MM:SS,sDD*MM:SS
  // NGC6813    ,DN ,19:41:08,+27*20:22
  v=server.arg("cu");
  if (v!="") {
    showMessage="";
    
    uploadCatalogData = true;
    if (currentCatalog > 0 && currentCatalog < 16) {
      sprintf(temp,":Lo%ld#",(long)currentCatalog-1);
      if (commandBool(temp)) {
        v.replace("_"," ");
        int lineNum = 0;
        if (v=="DELETE") { v=""; showMessage=L_CAT_DATA_REMOVED "."; }
        commandBlind(":LL#");  // clear this catalog
        while (v.length() > 0) { // any data left?
          lineNum++;
          String line,co,cat,ra,de;
          i=v.indexOf("\n"); if (i >= 0) { line = v.substring(0,i); v=v.substring(i+1); } else { line = v; v=""; }

          // catalog name?
          if (lineNum == 1) {
            line.trim();
            if (line.charAt(0) == '$') {
              co = line.substring(0);
              co.trim();
              if (co.length() < 2 || co.length() > 11) { showMessage=F(L_CAT_UPLOAD_FAIL); break; }
              if (!commandBool(":L$#")) { showMessage=F(L_CAT_UPLOAD_INDEX_FAIL); break; }
              if (!commandBool(":LD#")) { showMessage=F(L_CAT_DELETE_FAIL); break; }
              if (!commandBool((":LW"+co+"#").c_str())) { showMessage=F(L_CAT_WRITE_NAME_FAIL); break; }
              continue;
            } else { showMessage=L_CAT_UPLOAD_NO_NAME_FAIL; break; }
          }
          
          i=line.indexOf(","); if (i >= 0) { co  = line.substring(0,i); line=line.substring(i+1); } else { showMessage=F(L_CAT_BAD_FORM); showMessage+=String(lineNum); break; }
          i=line.indexOf(","); if (i >= 0) { cat = line.substring(0,i); line=line.substring(i+1); } else { showMessage=F(L_CAT_BAD_FORM); showMessage+=String(lineNum); break; }
          i=line.indexOf(","); if (i >= 0) { ra  = line.substring(0,i); line=line.substring(i+1); } else { showMessage=F(L_CAT_BAD_FORM); showMessage+=String(lineNum); break; }
          de = line;
          
          co.trim(); cat.trim(); ra.trim(); de.trim();
          if (co.length() < 1 || co.length() > 11) { showMessage=F(L_CAT_UPLOAD_BAD_OBJECT_NAME); showMessage+=String(lineNum); break; }
          if (cat != "UNK" && cat != "OC"  && cat != "GC"  && cat != "PN" && 
              cat != "DN"  && cat != "SG"  && cat != "EG"  && cat != "IG" &&
              cat != "KNT" && cat != "SNR" && cat != "GAL" && cat != "CN" &&
              cat != "STR" && cat != "PLA" && cat != "CMT" && cat != "AST") { showMessage=F(L_CAT_BAD_CATEGORY); showMessage+=String(lineNum); break; }

          if (!isDigit(ra.charAt(0)) || !isDigit(ra.charAt(1)) || 
              !isDigit(ra.charAt(3)) || !isDigit(ra.charAt(4)) ||
              !isDigit(ra.charAt(6)) || !isDigit(ra.charAt(7)) ||
              ra.charAt(2) != ':' || ra.charAt(5) != ':' || ra.length() != 8)  { showMessage=F(L_CAT_BAD_RA); showMessage+=String(lineNum); break; }
              
          if (!isDigit(de.charAt(1)) || !isDigit(de.charAt(2)) || 
              !isDigit(de.charAt(4)) || !isDigit(de.charAt(5)) ||
              !isDigit(de.charAt(7)) || !isDigit(de.charAt(8)) ||
              (de.charAt(0) != '+' && de.charAt(0) != '-') ||
              (de.charAt(3) != '*' && de.charAt(3) != ':') ||
              de.charAt(6) != ':' || de.length() != 9) { showMessage=F(L_CAT_BAD_DEC); showMessage+=String(lineNum); break; }

          if (!commandBool((":Sr"+ra+"#").c_str())) { showMessage=F(L_CAT_UPLOAD_RA_FAIL); showMessage+=String(lineNum); break; }
          if (!commandBool((":Sd"+de+"#").c_str())) { showMessage=F(L_CAT_UPLOAD_DEC_FAIL); showMessage+=String(lineNum); break; }
          if (!commandBool((":LW"+co+","+cat+"#").c_str())) { showMessage=F(L_CAT_UPLOAD_LINE_FAIL); showMessage+=String(lineNum); break; }
        }
        if (showMessage=="") showMessage=L_CAT_UPLOAD_SUCCESS ", "+String(lineNum)+" " L_CAT_UPLOAD_LINES_WRITTEN ".";
      } else showMessage=F(L_CAT_UPLOAD_SELECT_FAIL);
    } else showMessage=F(L_CAT_UPLOAD_NO_CAT);
  }

  // Catalog index (1-15)
  v=server.arg("ci");
  if (v != "") {
    currentCatalog=v.toInt();
    if (currentCatalog > 0 && currentCatalog < 16) {
      sprintf(temp,":Lo%ld#",(long)currentCatalog-1);
      if (!commandBool(temp)) currentCatalog=0;
    } else currentCatalog=0;
    catalogIndexChanged=true;
  }
}
