$(window).scroll(function (event) {
	var scroll = $(window).scrollTop();

	var shift = (-1) * (scroll/3.5) + "px";
	var opacity = scroll/700;

	$(".front").css("background-position-y", shift);
	$(".front-filter").css("opacity", opacity);
});

$(window).load(function() {
	$("#container1").twentytwenty();
});