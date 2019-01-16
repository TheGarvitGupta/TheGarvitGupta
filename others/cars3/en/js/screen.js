$(document).ready(function() {
	$(".play-button").click(function() {
		$(".screen").css("display", "block");
		$(".screen").css("opacity", "1");

		$("body").css("overflow", "hidden");

		var country;

		$.getJSON('https://ipinfo.io', function(data){
		
			if (data.country == "BH") {
				$(".video-container").html("<iframe class='video-youtube' frameborder='0' allowfullscreen='0' width='1920' height='1080' src='https://www.youtube.com/embed/O-yJ5jVSsD8?autoplay=1&controls=0&disablekb=1&fs=0&iv_load_policy=3&loop=1&playlist=O-yJ5jVSsD8&modestbranding=1&rel=0&showinfo=0'></iframe>");
			}
			else {
				$(".video-container").html("<iframe class='video-youtube' frameborder='0' allowfullscreen='0' width='1920' height='1080' src='https://www.youtube.com/embed/Amdgw4odTdM?autoplay=1&controls=0&disablekb=1&fs=0&iv_load_policy=3&loop=1&playlist=Amdgw4odTdM&modestbranding=1&rel=0&showinfo=0'></iframe>");
			}

		});
		
	});

	$(".screen-close-button").click(function() {
		$(".screen").css("opacity", "0");
		$("body").css("overflow", "initial");
		
		setTimeout(function() {
			$(".screen").css("display", "none");
			$(".video-container").html("");
    	}, 200);
	});
});