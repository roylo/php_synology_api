<?php

require_once dirname(__FILE__) . '/SynologyBaseAPI.inc';

class SynologyFileStationAPI extends SynologyBaseAPI
{
    public function __construct()
    {
        parent::__construct();
    }

    public function searchStart($folder_path, $opt_arr = null)
    { //{{{
        $api_name = "SYNO.FileStation.Search";
        $method = "start";
        $required_arr = array(
            "folder_path" => $folder_path,
        );

        return $this->_execute($api_name, $method, $required_arr, $opt_arr);
    } //}}}

    public function searchList($taskid, $opt_arr = null)
    { //{{{
        $api_name = "SYNO.FileStation.Search";
        $method = "list";
        $required_arr = array(
            "taskid" => $taskid,
        );

        return $this->_execute($api_name, $method, $required_arr, $opt_arr);
    } //}}}

    public function fileList($folder_path, $opt_arr = null)
    { //{{{
        $api_name = "SYNO.FileStation.List";
        $method = "list";
        $required_arr = array(
            "folder_path" => $folder_path,
        );

        return $this->_execute($api_name, $method, $required_arr, $opt_arr);
    } //}}}

    public function copyMoveStart($path, $dest_path, $remove_src = true, $opt_arr = null)
    { //{{{
        $api_name = "SYNO.FileStation.CopyMove";
        $method = "start";
        $required_arr = array(
            "path" => $path,
            "dest_folder_path" => $dest_path,
            "remove_src" => $remove_src,
        );

        return $this->_execute($api_name, $method, $required_arr, $opt_arr);
    } //}}}

    public function archiveList($path, $opt_arr = null)
    { //{{{
        $api_name = "SYNO.FileStation.Extract";
        $method = "list";
        $required_arr = array(
            "file_path" => $path,
        );
        return $this->_execute($api_name, $method, $required_arr, $opt_arr);
    } //}}}

    public function archiveExtract($path, $dest_path, $opt_arr = null)
    { //{{{
        $api_name = "SYNO.FileStation.Extract";
        $method = "start";
        $required_arr = array(
            "file_path" => $path,
            "dest_folder_path" => $dest_path,
        );
        return $this->_execute($api_name, $method, $required_arr, $opt_arr);
    } //}}}
}


