/*jslint browser:true */
/*jslint es5: true */
/*jslint nomen: true */
/*jslint white: true */
/*global $, jQuery, alert, console, _, FB*/

(function ($) {
	'use strict';

	var $form;

	$form = $('#form');

	function render() {
		var template;
		template = _.template($('#form-template').html());
		$form.html(template);
		console.log('Render form template complete');
	}

	function bindEvents() {

		function classicSubmit(event) {
			event.preventDefault();
			var ajaxData;

			ajaxData = {
			};
			console.log(ajaxData);
			$.ajax({
				url: '/signout/post/',
				type: 'POST',
				data: JSON.stringify(ajaxData),
				contentType: 'application/json; charset=utf-8',
				dataType: 'json',
				success: function () {
					console.log('Signup success');
				}
			});
			return false;
		}

		$form.submit(classicSubmit);
	}

	function preloadResources() {
		var imageUrls = [
		];

		/*jslint unparam: true*/
		$.each(imageUrls, function (i, imageUrl) {
			$("<img />").attr("src", imageUrl);
		});
		/*jslint unparam: false*/
	}

	function init() {
		preloadResources();
		render();
		bindEvents();
	}

	// Entry point
	init();

}(jQuery));
// vim: set tabstop=4 noexpandtab shiftwidth=4 softtabstop=4:
