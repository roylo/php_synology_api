<?php

require_once dirname(__FILE__) . '/../SynologyFileStationAPI.inc';

/*
$path = "/download/TVshows/test";
$dest_path = "/video";
$remove_src = true;
$opt_arr = array(
    "overwrite" => true,
);
$ret = $syno->copyMoveStart($path, $dest_path, $remove_src, $opt_arr);
*/

class TVShowHandler
{
    private $_syno;
    private $_recognize_archive_format;
    private $_filter_keywords;
    private $_want_keywords;

    public function __construct($syno = null, array $login_info)
    { //{{{
        if ($syno) {
            $this->_syno = $syno;
        } else {
            $this->_syno = new SynologyFileStationAPI();
        }

        $login_ret = $this->_syno->login(
            $login_info["account"],
            $login_info["passwd"],
            $login_info["session_name"],
            $login_info["cookie_file"]
        );

        if (!$login_ret) {
            die("Fail to login NAS!");
        }

        $this->_recognize_archive_format = array("ZIP", "RAR");
        $this->_filter_keywords = array("英文");
        $this->_want_keywords = array("繁体", "CHT");
    } //}}}

    public function processSubtitleFiles($folder_path)
    { //{{{
        // create processed folder for processed files dest
        if ($this->_syno->checkFileExist($folder_path, "processed", "dir") == false) {
            $create_folder_ret = $this->_syno->createFolder($folder_path, "processed");
        }
        $processed_folder_path = "$folder_path/processed";

        $folder_contents = $this->listFile($folder_path, "file");
        if ($folder_contents["success"] != true || count($folder_contents["data"]["files"]) <= 0) {
            return false;
        }

        $folder_files = $folder_contents["data"]["files"];
        // for each archive file, extract it, then move to junk folder
        foreach($folder_files as $file) {
            if (in_array($file["additional"]["type"], $this->_recognize_archive_format)) {
                $dest_path = "/video";
                $ret = $this->extractFile($file["path"], $dest_path, null);
                if ($ret) {
                    $opt_arr = array(
                        "overwrite" => true,
                    );
                    $this->_syno->StartCopyMove($file["path"], $processed_folder_path, true, $opt_arr);
                }
            }
        }
    } //}}}

    public function listFile($folder_path, $file_type)
    { //{{{
        $search_opt = array(
            "filetype" => $file_type,
            "additional" => "real_path,type",
        );
        $ret = $this->_syno->listFile($folder_path, $search_opt);
        return $ret;
    } //}}}

    public function extractFile($file_path, $dest_path, $keyword = null)
    { //{{{
        $opt_arr = array(
            "item_id" => -1,
        );
        $archive_list = $this->_syno->listArchive($file_path, $opt_arr);
        if ($archive_list["success"] == true && count($archive_list["data"]["items"]) > 0) {
            foreach ($archive_list["data"]["items"] as $item) {
                if ($this->_isWantFile($item["name"], "srt")) {
                    $extract_ret = $this->_doExtract($file_path, $dest_path, $item["item_id"]);
                    //TODO: rename file
                    return $extract_ret;
                }
            }
        }

        return false;
    } //}}}

    private function _isWantFile($file_name, $ext_type)
    { //{{{
        $ext = end(explode('.', $file_name));
        if ($ext != $ext_type) {
            return false;
        }

        foreach($this->_filter_keywords as $filter_keyword) {
            if (strpos($file_name, $filter_keyword) !== false) {
                continue;
            }

            foreach($this->_want_keywords as $keyword) {
                if (strpos($file_name, $keyword) !== false) {
                    return true;
                }
            }
        }
        return false;
    } //}}}

    private function _doExtract($file_path, $dest_path, $item_id = null)
    { //{{{
        $opt_arr = array(
            "overwrite" => true,
        );
        if ($item_id) {
            $opt_arr["item_id"] = $item_id;
        }

        $start_ret = $this->_syno->extractArchive($file_path, $dest_path, $opt_arr);
        if ($start_ret["success"] == true && isset($start_ret["data"]["taskid"])) {
            do {
                $status_ret = $this->_syno->getArchiveStatus($start_ret["data"]["taskid"]);
                sleep(1);
            } while($status_ret["data"]["finished"] != true);
        } else {
            return false;
        }
        return $status_ret["data"]["finished"];
    } //}}}
}
