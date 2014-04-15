<?php

require_once dirname(__FILE__) . '/SynologyBaseAPI.inc';

class SynologyFileStationAPI extends SynologyBaseAPI
{
    public function __construct()
    {
        parent::__construct();
    }

    public function search($folder_path, $opt_parameters = null)
    { //{{{
        $api_name = "SYNO.FileStation.Search";
        $method = "start";
        $this->_get_basic_info($api_name, $version, $path);
        $req = $this->_init_req($api_name, $version, $method);

        $req["folder_path"] = $folder_path;
        if (is_array($opt_parameters)) {
            $req = array_merge($req, $opt_parameters);
        }
        $result = $this->_ws_client->get("webapi/$path", $req);
        return $result;
    } //}}}

    public function searchList($taskid, $opt_parameters = null)
    { //{{{
        $api_name = "SYNO.FileStation.Search";
        $method = "list";
        $this->_get_basic_info($api_name, $version, $path);
        $req = $this->_init_req($api_name, $version, $method);
        $req["taskid"] = $taskid;

        if (is_array($opt_parameters)) {
            $req = array_merge($req, $opt_parameters);
        }
        $result = $this->_ws_client->get("webapi/$path", $req);
        return $result;
    } //}}}
}


