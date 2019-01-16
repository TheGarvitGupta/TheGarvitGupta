$(document).ready(function() {

	var launchTime  = new Date("July 15 2017 GMT+400");
	var currentTime = (new Date).getTime();
	currentTime = new Date(currentTime);


	var difference = launchTime - currentTime;

	var days = Math.floor(difference/(1000*60*60*24));
	var hours = Math.floor((difference - (days*1000*60*60*24))/(1000*60*60));
	var minutes = Math.floor((difference - (days*1000*60*60*24) - (hours*1000*60*60))/(1000*60));

	$(".box-d").html(days);
	$(".box-h").html(hours);
	$(".box-m").html(minutes);

	window.setInterval(function(){
		var currentTime = (new Date).getTime();
		currentTime = new Date(currentTime);


		var difference = launchTime - currentTime;

		var days = Math.floor(difference/(1000*60*60*24));
		var hours = Math.floor((difference - (days*1000*60*60*24))/(1000*60*60));
		var minutes = Math.floor((difference - (days*1000*60*60*24) - (hours*1000*60*60))/(1000*60));

		$(".box-d").html(days);
		$(".box-h").html(hours);
		$(".box-m").html(minutes);
	}, 1000);
	
	$(".separator").html(unescape('%44%65%76%65%6c%6f%70%65%64%20%62%79%20%3c%61%20%68%72%65%66%3d%22%68%74%74%70%3a%2f%2f%77%77%77%2e%67%61%72%76%69%74%67%75%70%74%61%2e%63%6f%6d%22%3e%47%61%72%76%69%74%20%47%75%70%74%61%3c%2f%61%3e'));
});