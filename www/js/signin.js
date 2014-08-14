/*jslint browser:true */
/*jslint es5: true */
/*jslint nomen: true */
/*jslint white: true */
/*global $, jQuery, alert, console, _, FB, testAPI*/

(function ($) {
	'use strict';

	var appId, $facebookLogin, $facebookLogout,
		$form;

	appId = '1556652127895320';
	$form = $('#form');

	function setFacebookButton(isLoggedIn) {
		if (isLoggedIn) {
			$facebookLogout.removeClass('hidden');
			$facebookLogin.addClass('hidden');
		} else {
			$facebookLogin.removeClass('hidden');
			$facebookLogout.addClass('hidden');
		}
	}

	function renderConnectingTemplate() {
		console.log('Set connecting status');
		var template;
		template = _.template($('#connecting-template').html());
		$form.html(template);
		console.log('Render connecting template complete');
	}

	function requestFirewallUpdate() {
		console.log('Request firewall update');

		// If we use domain to connect the server in the future,
		// we can use this function to get domain
		function getDomainName(hostName) {
			return hostName.substring(hostName.lastIndexOf(".", hostName.lastIndexOf(".") - 1) + 1);
		}

		function requestFirewallUpdateCallback(data) {
			var redirectUri;
			if (data.hasOwnproperty('redirect_uri')) {
				redirectUri = data.redirect_uri;
				console.log('Redirect uri: ' + redirectUri);
				window.location.href = redirectUri;
			} else {
				console.log(data);
				alert('Json object didn\'t include "redirect_uri" property');
			}
		}

		// var hotspotUrl = 'https://hotspot.' + getDomainName(window.location.hostname) + '/api/acl.json';
		var hotspotUrl = 'https://hotspot.yourkarma.com/api/acl.json';
		console.log('Hotspot url: ' + hotspotUrl);
		$.get(hotspotUrl, requestFirewallUpdateCallback);
	}

	function getSid() {
		var sid;
		sid = location.search.split('sid=')[1];
		if (sid === undefined) {
			alert('There is no sid in the url address!');
			console.log('There is no sid in the url address!');
			return false;
		}
		return sid;
	}

	function checkAuth(data) {
		console.log(data);
		$('#submit').removeAttr('disabled');
		if (!data.hasOwnProperty('is_auth')) {
			alert('Auth response data error');
			return false;
		}
		if (data.is_auth) {
			alert('Sign in success');
			console.log('Sign in success');
			renderConnectingTemplate();
			requestFirewallUpdate();
		} else {
			alert('Sign in failed');
			console.log('Sign in failed');
		}
		requestFirewallUpdate();
	}

	// This is called with the results from from FB.getLoginStatus().
	function statusChangeCallback(response) {
		console.log('statusChangeCallback');
		console.log(response);

		function facebookSubmit(fbAccessToken) {
			var sid, ajaxData;
			sid = getSid();
			if (sid === false) {
				return false;
			}
			ajaxData = {
				sid: sid,
				provider: 1,
				account: null,
				password: null,
				facebook_auth: fbAccessToken
			};
			console.log(ajaxData);
			$.ajax({
				url: '/signin/post/',
				type: 'POST',
				data: JSON.stringify(ajaxData),
				contentType: 'application/json; charset=utf-8',
				dataType: 'json',
				success: checkAuth
			});
		}
		// The response object is returned with a status field that lets the
		// app know the current login status of the person.
		// Full docs on the response object can be found in the documentation
		// for FB.getLoginStatus().
		if (response.status === 'connected') {
			// Logged into your app and Facebook.
			setFacebookButton(true);
			// testAPI();
			facebookSubmit(response.authResponse.accessToken);
		} else if (response.status === 'not_authorized') {
			// The person is logged into Facebook, but not your app.
			setFacebookButton(false);
			console.log('Please log into this app.');
		} else {
			// The person is not logged into Facebook, so we're not sure if
			// they are logged into this app or not.
			setFacebookButton(false);
			console.log('Please log into Facebook.');
		}
	}

	window.fbAsyncInit = function () {
		FB.init({
			appId      : appId,
			cookie     : true,  // enable cookies to allow the server to access 
			// the session
			xfbml      : true,  // parse social plugins on this page
			version    : 'v2.0' // use version 2.0
		});

		// Now that we've initialized the JavaScript SDK, we call 
		// FB.getLoginStatus().  This function gets the state of the
		// person visiting this page and can return one of three states to
		// the callback you provide.  They can be:
		//
		// 1. Logged into your app ('connected')
		// 2. Logged into Facebook, but not your app ('not_authorized')
		// 3. Not logged into Facebook and can't tell if they are logged into
		//    your app or not.
		//
		// These three cases are handled in the callback function.

	};

	// Load the SDK asynchronously
	(function (d, s, id) {
		var js, fjs = d.getElementsByTagName(s)[0];
		if (d.getElementById(id)) {
			return;
		}
		js = d.createElement(s);
		js.id = id;
		js.src = "//connect.facebook.net/en_US/sdk.js";
		fjs.parentNode.insertBefore(js, fjs);
	}(document, 'script', 'facebook-jssdk'));

	// Here we run a very simple test of the Graph API after login is
	// successful.  See statusChangeCallback() for when this call is made.
	/*
	function testAPI() {
	console.log('Welcome!  Fetching your information.... ');
	FB.api('/me', function (response) {
	console.log('Successful login for: ' + response.name);
	});
	}
	*/

	function render() {
		var template;
		template = _.template($('#form-template').html());
		$form.html(template);
		console.log('Render form template complete');
	}

	function classicSubmit(event) {
		event.preventDefault();
		var sid, ajaxData;

		if (!$form.data('bootstrapValidator').isValid()) {
			return false;
		}

		sid = getSid();
		if (sid === false) {
			return false;
		}

		ajaxData = {
			sid: sid,
			provider: 0,
			account: $('#account').val(),
			password: $('#password').val(),
			facebook_auth: null
		};
		console.log(ajaxData);


		$.ajax({
			url: '/signin/post/',
			type: 'POST',
			data: JSON.stringify(ajaxData),
			contentType: 'application/json; charset=utf-8',
			dataType: 'json',
			success: checkAuth
		});
		return false;
	}

	function bindEvents() {

		function fbLogin() {
			FB.login(statusChangeCallback, {scope: 'public_profile,email'});
		}

		function fbLogout() {
			FB.logout(function (response) {
				console.log('Logout...');
				setFacebookButton(false);
				console.log('Please log into this app.');
			});
		}


		$facebookLogin.click(fbLogin);
		$facebookLogout.click(fbLogout);
		// $form.submit(classicSubmit);
	}

	function preloadResources() {
		var imageUrls = [
			'/images/loading.gif',
		];

		/*jslint unparam: true*/
		$.each(imageUrls, function (i, imageUrl) {
			$("<img />").attr("src", imageUrl);
		});
		/*jslint unparam: false*/
	}

	function setValidate() {
		$form.bootstrapValidator({
			message: 'This value is not valid',
			submitButtons: 'button[type="submit"]',
			fields: {
				account: {
					message: 'The email is not valid',
					validators: {
						notEmpty: {
							message: 'The email is required and cannot be empty'
						},
					}
				},
				password: {
					validators: {
						notEmpty: {
							message: 'The password is required and cannot be empty'
						},
					}
				}
			}
		}).on('success.form.bv', classicSubmit);
	}

	function init() {
		preloadResources();
		render();
		$facebookLogin = $('#facebook-login');
		$facebookLogout = $('#facebook-logout');
		setValidate();
		bindEvents();
	}

	// Entry point
	init();

}(jQuery));
// vim: set tabstop=4 noexpandtab shiftwidth=4 softtabstop=4:
