function showResponse() {     
	$("#response-parent").html('<div class="animated zoomInUp" id="message-received-parent"><div class="message-received-child">Awesome! Your message has been sent.</div></div>');
}

/* Intent Select */

$(".checkimage-app" ).click(function toggleApp() {

	if (intentApp == 0)
	{
		intentApp = 1;
		$(".checkimage-app" ).css("background-color","#E1F5FE");
		$(".checkimage-app" ).css("border-color","#2196F3");
	}
	else
	{
		intentApp = 0;
		$(".checkimage-app" ).css("background-color","#ffffff");
		$(".checkimage-app" ).css("border-color","#000000");
	}

});

$(".checkimage-web" ).click(function toggleApp() {

	if (intentWeb == 0)
	{
		intentWeb = 1;
		$(".checkimage-web" ).css("background-color","#E1F5FE");
		$(".checkimage-web" ).css("border-color","#2196F3");
	}
	else
	{
		intentWeb = 0;
		$(".checkimage-web" ).css("background-color","#ffffff");
		$(".checkimage-web" ).css("border-color","#000000");
	}

});

$(".checkimage-job" ).click(function toggleApp() {

	if (intentJob == 0)
	{
		intentJob = 1;
		$(".checkimage-job" ).css("background-color","#E1F5FE");
		$(".checkimage-job" ).css("border-color","#2196F3");
	}
	else
	{
		intentJob = 0;
		$(".checkimage-job" ).css("background-color","#ffffff");
		$(".checkimage-job" ).css("border-color","#000000");
	}

});

$(".checkimage-feedback" ).click(function toggleApp() {

	if (intentFeedback == 0)
	{
		intentFeedback = 1;
		$(".checkimage-feedback" ).css("background-color","#E1F5FE");
		$(".checkimage-feedback" ).css("border-color","#2196F3");
	}
	else
	{
		intentFeedback = 0;
		$(".checkimage-feedback" ).css("background-color","#ffffff");
		$(".checkimage-feedback" ).css("border-color","#000000");
	}

});

/* Send Button */

$( ".submit-parent" ).click(function() {
	

	//$("#submit-icon").css("background-image", "none");
	//$("#submit-icon").html('<svg class="spinner" style="margin-left:10px;margin-top:10px;" width="22px" height="22px" viewBox="0 0 66 66" xmlns="http://www.w3.org/2000/svg"><circle class="path" fill="none" stroke-width="6" stroke-linecap="round" cx="33" cy="33" r="30"></circle></svg>');
	
	$(".cssload-container").css("opacity", "1");

	var name = $('#contactName').val();
	var email = $('#contactEmail').val();;
	var number = $('#contactNumber').val();;
	var message = $('#contactMessage').val();;

	xmlhttp = new XMLHttpRequest();
	xmlhttp.onreadystatechange = function () {
		if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
			var Reply = xmlhttp.responseText;
			if(Reply == '1')
			{	
				$(".contact-form").addClass("bounceOutUp");
				setTimeout(showResponse, 1000);
			}
			else
			{
				alert("We detected an error in the input of the form. Did you miss filling in some detail?")
				$("#submit-icon").html('');
				$("#submit-icon").css("background-image", "url('images/icons/right.png')");
			}
		}
	}
	xmlhttp.open("POST","php/receiveMessage.php",true);
	xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xmlhttp.send("Name=" + name + "&Email=" + email + "&Number=" + number + "&Message=" + message + "&App=" + intentApp + "&Web=" + intentWeb + "&Job=" + intentJob + "&Feedback=" + intentFeedback);
	xmlhttp.onerror = function(){
		alert("We detected an error in the input of the form. Did you miss filling in some detail?")
		$(".cssload-container").css("opacity", "0");
	};
});