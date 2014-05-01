<?php

require_once dirname(__FILE__) . '/SynologyBaseAPI.inc';

class SynologyFileStationAPI extends SynologyBaseAPI
{
    public function __construct()
    {
        parent::__construct();
    }

    public function startSearch($folder_path, $opt_arr = null)
    { //{{{
        $api_name = "SYNO.FileStation.Search";
        $method = "start";
        $required_arr = array(
            "folder_path" => $folder_path,
        );

        return $this->_execute($api_name, $method, $required_arr, $opt_arr);
    } //}}}

    public function listSearch($taskid, $opt_arr = null)
    { //{{{
        $api_name = "SYNO.FileStation.Search";
        $method = "list";
        $required_arr = array(
            "taskid" => $taskid,
        );

        return $this->_execute($api_name, $method, $required_arr, $opt_arr);
    } //}}}

    public function listFile($folder_path, $opt_arr = null)
    { //{{{
        $api_name = "SYNO.FileStation.List";
        $method = "list";
        $required_arr = array(
            "folder_path" => $folder_path,
        );

        return $this->_execute($api_name, $method, $required_arr, $opt_arr);
    } //}}}

    public function StartCopyMove($path, $dest_path, $remove_src = true, $opt_arr = null)
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

    public function listArchive($path, $opt_arr = null)
    { //{{{
        $api_name = "SYNO.FileStation.Extract";
        $method = "list";
        $required_arr = array(
            "file_path" => $path,
        );
        return $this->_execute($api_name, $method, $required_arr, $opt_arr);
    } //}}}

    public function extractArchive($path, $dest_path, $opt_arr = null)
    { //{{{
        $api_name = "SYNO.FileStation.Extract";
        $method = "start";
        $required_arr = array(
            "file_path" => $path,
            "dest_folder_path" => $dest_path,
        );
        return $this->_execute($api_name, $method, $required_arr, $opt_arr);
    } //}}}

    public function getArchiveStatus($task_id, $opt_arr = null)
    { //{{{
        $api_name = "SYNO.FileStation.Compress";
        $method = "status";
        $required_arr = array(
            "taskid" => $task_id,
        );
        return $this->_execute($api_name, $method, $required_arr, $opt_arr);
    } //}}}

    public function createFolder($folder_path, $name, $opt_arr = null)
    { //{{{
        $api_name = "SYNO.FileStation.CreateFolder";
        $method = "create";
        $required_arr = array(
            "folder_path" => $folder_path,
            "name"        => $name,
        );
        return $this->_execute($api_name, $method, $required_arr, $opt_arr);
    } //}}}

    public function checkFileExist($folder_path, $name, $is_dir = false)
    { //{{{
        $search_opt = array(
            "filetype" => ($is_dir == true) ? "dir" : "file",
            "additional" => "real_path,type",
        );
        $ret = $this->listFile($folder_path, $search_opt);

        if (!$ret["success"]) {
            return false;
        }

        foreach($ret["data"]["files"] as $file) {
            if ($file["isdir"] != $is_dir) {
                continue;
            }
            if ($file["name"] == $name) {
                return true;
            }
        }

        return false;
    } //}}}

    public function deleteFile($path, $opt_arr = null)
    { //{{{
        $api_name = "SYNO.FileStation.Delete";
        $method = "start";
        $required_arr = array(
            "path" => $path,
        );
        return $this->_execute($api_name, $method, $required_arr, $opt_arr);
    } //}}}
}


