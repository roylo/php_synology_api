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
    private static $_recognize_archive_format = array("ZIP", "RAR");
    private static $_keep_file_format = array("SRT", "AVI", "MP4");
    private static $_filter_keywords = array("英文", "荎恅");
    private static $_want_keywords = array("繁体", "CHT", "楛极");
    const VIDEO_SIZE = 50000000;

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

//        $this->_recognize_archive_format = array("ZIP", "RAR");
//        $this->_filter_keywords = array("英文", "荎恅");
//        $this->_want_keywords = array("繁体", "CHT", "楛极");
    } //}}}

    public function dispatchTvShows($download_folder, $delete_empty = false)
    { //{{{
        $files = $this->listFile($download_folder, "all");
        if (count($files['data']['files']) <= 0) {
            if ($delete_empty) {
                $this->_syno->deleteFile($download_folder);
            }
            return true;
        }
        foreach ($files['data']['files'] as $file) {
            if ($file['isdir']) {
                $this->dispatchTvShows($file['path'], true);
            } else {
                $isVideoFile = $this->_matchTvPattern($file);
                if (!$isVideoFile) {
                    if (!in_array($file['additional']['type'], self::$_keep_file_format)
                        && $file['additional']['size'] < self::VIDEO_SIZE) {
                        $this->_syno->deleteFile($file["path"]);
                    }
                }
            }

        }
    } //}}}

    private function _matchTvPattern($file)
    { //{{{
        $pattern = "/([\w.]+)([sS]\d{2})([eE]\d{2})(.*)/";
        $ret = preg_match($pattern, $file['name'], $matches);
        if ($ret) {
            $name = $matches[1];
            $season_str = $matches[2];
            $episode_str = $matches[3];
            if (substr($name, -1) == ".") {
                $name = substr($name, 0, -1);
            }
            $dest_path = $this->_findDestPath($name, $season_str, $episode_str);
            $opt_arr = array(
                "overwrite" => true,
            );
            $this->_syno->StartCopyMove($file["path"], $dest_path, true, $opt_arr);
            return true;
        }

        return false;
    } //}}}

    public function processSubtitleFiles($folder_path)
    { //{{{
        // create processed folder for processed files dest
        $this->_createFolderIfNotExist($folder_path, "processed");
        $processed_folder_path = "$folder_path/processed";

        $folder_contents = $this->listFile($folder_path, "file");
        if ($folder_contents["success"] != true || count($folder_contents["data"]["files"]) <= 0) {
            return false;
        }

        $folder_files = $folder_contents["data"]["files"];
        // for each archive file, extract it, then move to junk folder
        foreach($folder_files as $file) {
            if (in_array($file["additional"]["type"], self::$_recognize_archive_format)) {
                list($show_name, $season_str, $episode_str) = explode("+", $file['name']);
                $dest_path = $this->_findDestPath($show_name, $season_str, $episode_str);
                $ret = $this->extractFile($file["path"], $dest_path);
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
            "additional" => "real_path,type,size",
        );
        $ret = $this->_syno->listFile($folder_path, $search_opt);
        return $ret;
    } //}}}

    public function extractFile($file_path, $dest_path)
    { //{{{
        $want_item_id = $this->_findWantItemId($file_path);
        echo "$file_path\t$want_item_id\n";


        $extract_ret = $this->_doExtract($file_path, $dest_path, $want_item_id);
        //TODO: rename file
        return $extract_ret;
    } //}}}

    private function _findWantItemId($file_path, $init_item_id = -1)
    { //{{{
        $dir_items = array();
        do {
            $dir_item_id = array_shift($dir_items);
            if ($dir_item_id !== null) {
                $item_id = $dir_item_id;
            } else {
                $item_id = $init_item_id;
            }

            $opt_arr = array(
                "item_id" => $item_id,
            );

            $archive_list = $this->_syno->listArchive($file_path, $opt_arr);
            if ($archive_list["success"] == false || count($archive_list["data"]["items"]) == 0) {
                return false;
            }

            foreach ($archive_list["data"]["items"] as $item) {
                if ($item["is_dir"] == 1 && $item["item_id"] >= 0) {
                    array_push($dir_items, $item["item_id"]);
                } else {
                    if ($this->_isWantFile($item["name"], "srt")) {
                        return $item["item_id"];
                    }
                }
            }
        } while(count($dir_items) > 0);
        return false;
    } //}}}

    private function _isWantFile($file_name, $ext_type)
    { //{{{
        $ext = end(explode('.', $file_name));
        if ($ext != $ext_type) {
            return false;
        }

        foreach(self::$_filter_keywords as $filter_keyword) {
            if (strpos($file_name, $filter_keyword) !== false) {
                return false;
            }
        }

        foreach(self::$_want_keywords as $keyword) {
            if (strpos($file_name, $keyword) !== false) {
                return true;
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

    private function _initSubtileFolder($base_path, $folder_name)
    { //{{{
        $is_folder_exist = $this->_syno->checkFileExist($base_path, $folder_name, true);
        if (!$is_folder_exist) {
            $ret = $this->_syno->createFolder($base_path, $folder_name);
        }
        return $ret;
    } //}}}

    private function _findDestPath($show_name, $season_str, $episode_str, $create_folder = true)
    { //{{{
        $show_name = strtolower($show_name);
        $season_str = strtolower($season_str);
        $episode_str = strtolower($episode_str);
        $season_num = intval(substr($season_str, strrpos($season_str, "s") + 1));
        $episode_num = intval(substr($episode_str, strrpos($episode_str, "s") + 1));
        $folder_name = "season$season_num";
        $parent_folder = "/video/AmericanDrama/Ongoing/$show_name";
        if ($create_filder) {
            $this->_createFolderIfNotExist($parent_folder, $folder_name);
        }
        $dest_path = "$parent_folder/$folder_name";

        return $dest_path;
    } //}}}

    private function _createFolderIfNotExist($folder_path, $check_name)
    { //{{{
        $opt = array("force_parent" => true);
        if ($this->_syno->checkFileExist($folder_path, $check_name, "dir") == false) {
            $create_folder_ret = $this->_syno->createFolder($folder_path, $check_name, $opt);
        }
    } //}}}
}
