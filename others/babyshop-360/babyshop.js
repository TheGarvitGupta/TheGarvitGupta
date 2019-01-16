/* Fullscreen toggle */

$(document).ready(function() {
  $(window).on('resize', function(){
    if((window.fullScreen) || (window.innerWidth == screen.width && window.innerHeight == screen.height)) {
      $("#enter-fs").css("display", "none");
      $("#exit-fs").css("display", "inline-block");
    }
    else {
      $("#exit-fs").css("display", "none");
      $("#enter-fs").css("display", "inline-block");
    }
  });
});

/* Links */

$(document).ready(function() {
  $(".info-hotspot-text").each(function(index) {
    $(this).html($(this).html() + "<a target='_blank' class='shopLink' href='" + link[index] + "'>Shop Now</a>");
  });
});

/* Close all other hotspots except selected */

$(document).ready(function() {
	$('.hotspot').click(function(){
		/* All hotspots */
		$('.hotspot').not(this).removeClass("visible");
		$('.hotspot').css("z-index", "4999");
		
		/* Current hotspot */
		$(this).css("z-index", "5000");
	});
	$('.hotspot').hover(function(){
		/* All hotspots */
		$('.hotspot').not(this).removeClass("visible");
		$('.hotspot').css("z-index", "4999");
		
		/* Current hotspot */
		$(this).css("z-index", "5000");
	});
});

/* Fix height for double line titles */
$(document).ready(function() {
	$(".info-hotspot-text").each(function(index) {
		if (index == 2 || index == 6 || index == 7 || index == 8 || index == 10 || index == 14 || index == 15 || index == 17 || index == 21 || index == 22 || index == 24 || index == 25) {
			$(this).css("top", "50px");
		}
	});
});


/* Add brand logos */
	$(".info-hotspot-text").each(function(index) {
		if (index == 2) {
			$(this).addClass("barbie");
		}

		else if (index == 0 || index == 15 || index == 19 || index == 22) {
			$(this).addClass("lee");
		}
	});

/* Left/Right Buttons Manager */

$(document).ready(function() {
	$('.l-arrow').click(function() {

		var index = $(this).index() + 1;
		$(".l-arrow:nth-child(" + index + ")").removeClass("toShow");

		if (index == 1) {
			$(".l-arrow:nth-child(5)").addClass("toShow");
			$(".r-arrow:nth-child(3)").removeClass("toShow");
			$(".r-arrow:nth-child(2)").addClass("toShow");
		}

		if (index == 2) {
			$(".l-arrow:nth-child(1)").addClass("toShow");
			$(".r-arrow:nth-child(4)").removeClass("toShow");
			$(".r-arrow:nth-child(3)").addClass("toShow");
		}

		if (index == 3) {
			$(".l-arrow:nth-child(2)").addClass("toShow");
			$(".r-arrow:nth-child(5)").removeClass("toShow");
			$(".r-arrow:nth-child(4)").addClass("toShow");
		}

		if (index == 4) {
			$(".l-arrow:nth-child(3)").addClass("toShow");
			$(".r-arrow:nth-child(1)").removeClass("toShow");
			$(".r-arrow:nth-child(5)").addClass("toShow");
		}

		if (index == 5) {
			$(".l-arrow:nth-child(4)").addClass("toShow");
			$(".r-arrow:nth-child(2)").removeClass("toShow");
			$(".r-arrow:nth-child(1)").addClass("toShow");
		}
	});	


	$('.r-arrow').click(function() {

		var index = $(this).index() + 1;
		$(".r-arrow:nth-child(" + index + ")").removeClass("toShow");

		if (index == 1) {
			$(".r-arrow:nth-child(2)").addClass("toShow");
			$(".l-arrow:nth-child(4)").removeClass("toShow");
			$(".l-arrow:nth-child(5)").addClass("toShow");
		}

		if (index == 2) {
			$(".r-arrow:nth-child(3)").addClass("toShow");
			$(".l-arrow:nth-child(5)").removeClass("toShow");
			$(".l-arrow:nth-child(1)").addClass("toShow");
		}

		if (index == 3) {
			$(".r-arrow:nth-child(4)").addClass("toShow");
			$(".l-arrow:nth-child(1)").removeClass("toShow");
			$(".l-arrow:nth-child(2)").addClass("toShow");
		}

		if (index == 4) {
			$(".r-arrow:nth-child(5)").addClass("toShow");
			$(".l-arrow:nth-child(2)").removeClass("toShow");
			$(".l-arrow:nth-child(3)").addClass("toShow");
		}

		if (index == 5) {
			$(".r-arrow:nth-child(1)").addClass("toShow");
			$(".l-arrow:nth-child(3)").removeClass("toShow");
			$(".l-arrow:nth-child(4)").addClass("toShow");
		}
	});	

	$('.dot-1').click(function() {
			$(".l-arrow").each(function(index) {
				$(this).removeClass("toShow");
			});
			$(".r-arrow").each(function(index) {
				$(this).removeClass("toShow");
			});			
			$(".r-arrow:nth-child(2)").addClass("toShow");
			$(".l-arrow:nth-child(5)").addClass("toShow");
	});

	$('.dot-2').click(function() {
			$(".l-arrow").each(function(index) {
				$(this).removeClass("toShow");
			});
			$(".r-arrow").each(function(index) {
				$(this).removeClass("toShow");
			});			
			$(".r-arrow:nth-child(3)").addClass("toShow");
			$(".l-arrow:nth-child(1)").addClass("toShow");
	});

	$('.dot-3').click(function() {
			$(".l-arrow").each(function(index) {
				$(this).removeClass("toShow");
			});
			$(".r-arrow").each(function(index) {
				$(this).removeClass("toShow");
			});			
			$(".r-arrow:nth-child(4)").addClass("toShow");
			$(".l-arrow:nth-child(2)").addClass("toShow");
	});

	$('.dot-4').click(function() {
			$(".l-arrow").each(function(index) {
				$(this).removeClass("toShow");
			});
			$(".r-arrow").each(function(index) {
				$(this).removeClass("toShow");
			});			
			$(".r-arrow:nth-child(5)").addClass("toShow");
			$(".l-arrow:nth-child(3)").addClass("toShow");
	});

	$('.dot-5').click(function() {
			$(".l-arrow").each(function(index) {
				$(this).removeClass("toShow");
			});
			$(".r-arrow").each(function(index) {
				$(this).removeClass("toShow");
			});			
			$(".r-arrow:nth-child(1)").addClass("toShow");
			$(".l-arrow:nth-child(4)").addClass("toShow");
	});
});

	$(document).ready(function() {
		var isTouch =  !!("ontouchstart" in window) || window.navigator.msMaxTouchPoints > 0;
		if( !isTouch ){
	    	$('.hotspot').hover(function(){
				$(this).addClass("visible");
			},function(){
				$(this).removeClass("visible");
			});		
		}
	});

//Disable full screen
jQuery(document).ready(function($){
	var deviceAgent = navigator.userAgent.toLowerCase();
	var agentID = deviceAgent.match(/(iphone|ipod|ipad)/);
	if (agentID) {
		$('.screen-button').css("display","none");
	}
});