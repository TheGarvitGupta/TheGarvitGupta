var refreshTry = true;
var access_token = "null";
var refresh_token = "null";
$.ajaxSetup({ cache: false });

$(document).ready(function() {

	$.ajax({
		url: 'https://www.garvitgupta.com/spotify/get_spotify_access_token.php',
		success: function(data) {
			access_token = data;
			console.log("Access token: " + access_token);

			$.ajax({
				url: 'https://www.garvitgupta.com/spotify/get_spotify_refresh_token.php',
				success: function(data) {
					refresh_token = data;
					console.log("Refresh token: " + refresh_token);
					
					fetchOnceTrackData(access_token, refresh_token);
				}
			});
		}
	});
});

function fetchOnceTrackData(access_token, refresh_token) {

	console.log("Attempting to get track information using access token: " + access_token);

	$.ajax({
		url: 'https://api.spotify.com/v1/me/player/currently-playing',
		headers: {
			'Authorization':'Bearer ' + access_token
		},
		method: 'GET',

		success: function(data){
			updateUINow(data);
			console.log('Success - fetch once sucessful, token working fine. Track update starting with ' + access_token);
			updateTrackUI(access_token, refresh_token);
		},
		error: function(data) {
			console.log("Could not use the access token to fetch track details.");
			
			if (refreshTry) {
				refreshTry = false;
				console.log("Attempting to refresh the access token...");
				refreshAccessToken(refresh_token);
			}

			$.get("https://www.garvitgupta.com/spotify/get_spotify_access_token.php", function( data ) {
				access_token = data;
				console.log("Access token now is " + access_token);

				$.get("https://www.garvitgupta.com/spotify/get_spotify_refresh_token.php", function( data ) {
					refresh_token = data;

					fetchOnceTrackData(access_token, refresh_token);
				});
			});
		}
  	});
}

function refreshAccessToken(refresh_token) {

	$.post("https://accounts.spotify.com/api/token",
		{
			grant_type: "refresh_token",
			refresh_token: refresh_token,
			client_id: "6f9f2995dc574ebcaea5eb13669f0802",
			client_secret: "79eb037afe8647ee9d5e28cfc16e82f9"
		},
		function(data, status){
			var access_token = data.access_token;
			updateAccessToken(access_token);

			var refresh_token = data.refresh_token;
			if (refresh_token != undefined) {
				updateRefreshToken(refresh_token);
			}
		});
}

function updateTrackUI(access_token, refresh_token) {
	console.log("Yey");
	window.setInterval(function(){
			
		$.ajax({
			url: 'https://api.spotify.com/v1/me/player/currently-playing',
			headers: {
				'Authorization':'Bearer ' + access_token
			},
			method: 'GET',

			success: function(data){
				console.log('success: ' + JSON.stringify(data));
				if (JSON.stringify(data) == undefined) {
					console.log("User is not playing anything right now.");

					// Hide spotify player
					$(".spotify").css({"opacity": "0"});
					setTimeout(function() {
				   		$(".spotify").css({"display": "none"});	
					}, 200);
				}
				else {
					updateUINow(data);
					console.log("Updated UI");
				}
			},
			error: function(data) {
				console.log("Couldn't update UI");
			}
		});
	}, 5000);
}

function updateAccessToken(access_token) {
	console.log("Updating access token");
	$.post("https://www.garvitgupta.com/spotify/update_spotify_access_token.php", {"access_token": access_token}).done(	function( data ) {
    	console.log("Response: " + data);
  	});
}

function updateRefreshToken(refresh_token) {
	console.log("Updating refresh token");
	$.post("https://www.garvitgupta.com/spotify/update_spotify_refresh_token.php", {"refresh_token": refresh_token});
}

function updateUINow(data) {
	$(".spotify-progress").css({"width": data.progress_ms * 100/data.item.duration_ms + "%"});
	$(".spotify-music-track").html(data.item.name);
	$(".spotify-music-artist").html(data.item.album.artists[0].name);
	$(".spotify-album-art").css({"background-image":"url(" + data.item.album.images[1].url + ")"});
	$(".spotify-link").attr("href", data.item.external_urls.spotify);
	$(".spotify").css({"display": "block"});
	setTimeout(function() {
   		$(".spotify").css({"opacity": "1"});
	}, 200);
}