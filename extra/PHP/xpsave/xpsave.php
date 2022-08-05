<?php

function xpsave_readconfig(
	$cfg = "/home/et/.etwolf/etpub/xpsave.cfg",
	$sort = "playerrating")
{

	if(!is_readable($cfg))
		return array();

	$file = file($cfg);
	while(list($lnum,$line) = each($file)) {
		$line = trim($line);
		if($line == "[xpsave]") {
			while(trim($line) != "") {
				list($lnum,$line) = each($file);
				if(trim($line) == "")
					continue;
				$lparts = explode("=", $line);
				if(@count($lparts) < 2) {
					?>
					<!--
					xpsave_readconfig: parse error
					on line <?php echo $lnum;?> near
					<?php echo htmlspecialchars($line);?>
					-->	
					<?php
					continue;
				}
				$p = trim(array_shift($lparts));
				$n = implode("", $lparts); 
				$xpsave[$p] = trim($n);
			}
			// tjw: old versions of xpsave lacked name
			if($xpsave["name"] == "")
				continue;
			if(!isset($xpsave["killrating"]))
				$xpsave["killrating"] = 1600;
			if(!isset($xpsave["playerrating"]))
				$xpsave["playerrating"] = 1600;
			$xpsaves[$xpsave["guid"]] = $xpsave;
			unset($xpsave);
		}
	}
	if(!is_array($xpsaves))
		return array();
	if($sort == "name") {
		$sort_ord = SORT_ASC;
		$sort_type = SORT_STRING;
	}
	else {
		$sort_ord = SORT_DESC;
		$sort_type = SORT_NUMERIC;
	}
	while (list(,$v) = each($xpsaves))
		$sort_ary[] = strtolower($v[$sort]);
	reset($xpsaves);
	array_multisort($sort_ary, $sort_type, $sort_ord, $xpsaves);
	return $xpsaves;
}
?>
