$(document).ready(function() {

	/* Terms and Conditions */

	$(".terms").click(function() {
		$(".modal-background").css("display", "block");
		$(".modal-background").css("opacity", "1");
		$("body").css("overflow", "hidden");
		
		$(".tnc").css("display", "block");
		$(".tnc").css("opacity", "1");
	});

	$(".prize").click(function() {
		$(".modal-background").css("display", "block");
		$(".modal-background").css("opacity", "1");
		$("body").css("overflow", "hidden");
		
		$(".prizes").css("display", "block");
		$(".prizes").css("opacity", "1");
	});

	$(".popups-close-button").click(function() {
		$(".tnc").css("opacity", "0");
		$(".prizes").css("opacity", "0");
		$(".modal-background").css("opacity", "0");
		$("body").css("overflow", "initial");
		
		setTimeout(function() {
			$(".tnc").css("display", "none");
			$(".prizes").css("display", "none");
			$(".modal-background").css("display", "none");
    	}, 200);
	});
});