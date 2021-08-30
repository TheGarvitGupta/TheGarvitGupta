<?php
	$file = fopen("spotify_refresh_token.txt", "r");
	$spotify_refresh_token = fgets($file);
	echo $spotify_refresh_token;
?>