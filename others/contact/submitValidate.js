function validateFields()
{
	/* Full Name */

	if ($('#c_name').val().length === 0 )
	{
		$("#c_name").css("border-color", "#ff0033");
		$("#c_name_error").css("display", "block");
		$("#c_name_error").html("Please enter your full name");
	}
	else 
	{
		$("#c_name_error").css("display", "none");
		$("#c_name").css("border-color", "#fff");
	}

	/* Business Email */
	if ($('#c_email').val().length === 0 )
	{
		$("#c_email").css("border-color", "#ff0033");
		$("#c_email_error").css("display", "block");
		$("#c_email_error").html("Please enter your business email address");
	}
	
	else if (!isEmail($('#c_email').val()))
	{
		$("#c_email").css("border-color", "#ff0033");
		$("#c_email_error").css("display", "block");
		$("#c_email_error").html("Please enter a valid email address");
	}
	
	else 
	{
		$("#c_email_error").css("display", "none");
		$("#c_email").css("border-color", "#fff");
	}
	
	/* Organization */

	if ($('#c_organization').val().length === 0 )
	{
		$("#c_organization").css("border-color", "#ff0033");
		$("#c_organization_error").css("display", "block");
		$("#c_organization_error").html("Please enter your organization");
	}
	else 
	{
		$("#c_organization_error").css("display", "none");
		$("#c_organization").css("border-color", "#fff");
	}	
	
	/* Job Title */

	if ($('#c_job').val().length === 0 )
	{
		$("#c_job").css("border-color", "#ff0033");
		$("#c_job_error").css("display", "block");
		$("#c_job_error").html("Please enter your job title");
	}
	else 
	{
		$("#c_job_error").css("display", "none");
		$("#c_job").css("border-color", "#fff");
	}	
	
	/* Phone Number */

	if ($('#c_phone').val().length === 0 )
	{
		$("#c_phone").css("border-color", "#ff0033");
		$("#c_phone_error").css("display", "block");
		$("#c_phone_error").html("Please enter your phone number");
	}
	else 
	{
		$("#c_phone_error").css("display", "none");
		$("#c_phone").css("border-color", "#fff");
	}
	
	/* Message */

	if ($('#txtInput').val().length === 0 )
	{
		$("#txtInput").css("border-color", "#ff0033");
		$("#c_txtInput_error").css("display", "block");
		$("#c_txtInput_error").html("Please enter your query");
	}
	else 
	{
		$("#c_txtInput_error").css("display", "none");
		$("#txtInput").css("border-color", "#fff");
	}		
	
	/* Country */

	if( !$('#c_country').val()) {
		$("#country-selector").css("cssText", "border-color: #ff0033 !important;");
		$("#c_country_error").css("display", "block");
		$("#c_country_error").html("Please select your country");
	}
	else {
		$("#country-selector").css("cssText", "border-color: #fff !important;");
		$("#c_country_error").css("display", "none");
	}
	
	/* Query Type */

	if( !$('#c_query').val()) {
		$("#query-selector").css("cssText", "border-color: #ff0033 !important;");
		$("#c_query_error").css("display", "block");
		$("#c_query_error").html("Please select your query type");
	}
	else {
		$("#query-selector").css("cssText", "border-color: #fff !important;");
		$("#c_query_error").css("display", "none");
	}
}