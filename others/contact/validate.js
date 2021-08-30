/* Full Name */

$("#c_name").focus(function(){
    
	$("#c_name").css("border-color", "#2196F3");
});

$("#c_name").blur(function(){
    if ($('#c_name').val().length === 0 )
	{
		$("#c_name").css("border-color", "#ff0033");
	}
	else 
	{
		$("#c_name_error").css("display", "none");
		$("#c_name").css("border-color", "#fff");
	}
});

/* Business Email */

$("#c_email").focus(function(){
	$("#c_email").css("border-color", "#2196F3");
});

$("#c_email").blur(function(){
    if ($('#c_email').val().length === 0 )
	{
		$("#c_email").css("border-color", "#ff0033");
		$("#c_email_error").html("Please enter your business email address");
	}
	
	else if (!isEmail($('#c_email').val()))
	{
		$("#c_email").css("border-color", "#ff0033");
		$("#c_email_error").html("Please enter a valid email address");
	}
	
	else 
	{
		$("#c_email_error").css("display", "none");
		$("#c_email").css("border-color", "#fff");
	}
});

/* Organization */

$("#c_organization").focus(function(){
    
	$("#c_organization").css("border-color", "#2196F3");
});

$("#c_organization").blur(function(){
    if ($('#c_organization').val().length === 0 )
	{
		$("#c_organization").css("border-color", "#ff0033");
	}
	else 
	{
		$("#c_organization_error").css("display", "none");
		$("#c_organization").css("border-color", "#fff");
	}
});

/* Job Title */

$("#c_job").focus(function(){
    
	$("#c_job").css("border-color", "#2196F3");
});

$("#c_job").blur(function(){
    if ($('#c_job').val().length === 0 )
	{
		$("#c_job").css("border-color", "#ff0033");
	}
	else 
	{
		$("#c_job_error").css("display", "none");
		$("#c_job").css("border-color", "#fff");
	}
});

/* Phone Number */

$("#c_phone").focus(function(){
    
	$("#c_phone").css("border-color", "#2196F3");
});

$("#c_phone").blur(function(){
    if ($('#c_phone').val().length === 0 )
	{
		$("#c_phone").css("border-color", "#ff0033");
	}
	else 
	{
		$("#c_phone_error").css("display", "none");
		$("#c_phone").css("border-color", "#fff");
	}	
});

/* Message */

$("#txtInput").focus(function(){
    
	$("#txtInput").css("border-color", "#2196F3");
});

$("#txtInput").blur(function(){
    if ($('#txtInput').val().length === 0 )
	{
		$("#txtInput").css("border-color", "#ff0033");
	}
	else 
	{
		$("#c_txtInput_error").css("display", "none");
		$("#txtInput").css("border-color", "#fff");
	}	
});

/* Country */

$("#c_country").focus(function(){
	$("#c_country").css("border-color", "#2196F3");
});

$("#c_country").blur(function(){
    if( !$('#c_country').val() )
	{
		$("#c_country").css("border-color", "#ff0033");
	}
	else 
	{
		$("#c_country_error").css("display", "none");
		$("#c_country").css("border-color", "#fff");
	}
});

/* Query Type */

$("#c_query").focus(function(){
	$("#c_query").css("border-color", "#2196F3");
});

$("#c_query").blur(function(){
	
    if( !$('#c_query').val() )
	{
		$("#c_query").css("border-color", "#ff0033");
	}
	else 
	{
		$("#c_query_error").css("display", "none");
		$("#c_query").css("border-color", "#fff");
	}
});





/* Functions */

function isEmail(email) {
	var regex = /^([a-zA-Z0-9_.+-])+\@(([a-zA-Z0-9-])+\.)+([a-zA-Z0-9]{2,4})+$/;
	return regex.test(email);
}

$(document).ready(function() {
    $("#c_phone").keydown(function (e) {
        // Allow: plus, backspace, delete, tab, escape, enter and .
        if ($.inArray(e.keyCode, [46, 8, 9, 27, 13, 110, 190]) !== -1 ||
             // Allow: Ctrl+A
            (e.keyCode == 65 && e.ctrlKey === true) ||
             // Allow: Ctrl+C
            (e.keyCode == 67 && e.ctrlKey === true) ||
             // Allow: Ctrl+X
            (e.keyCode == 88 && e.ctrlKey === true) ||
             // Allow: home, end, left, right
            (e.keyCode >= 35 && e.keyCode <= 39)) {
                 // let it happen, don't do anything
                 return;
        }
        // Ensure that it is a number and stop the keypress
        if ((e.shiftKey || (e.keyCode < 48 || e.keyCode > 57)) && (e.keyCode < 96 || e.keyCode > 105)) {
            e.preventDefault();
        }
    });
});