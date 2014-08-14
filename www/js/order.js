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

	function show_order(eason) {
		$('#submit').removeAttr('disabled');

		var i, data_len, trTemplate, tableTemplate, trSumTemplate, trResultTemplate,
			$queryTbl;

		data_len = eason.data.length;
		$queryTbl = $('#query_div');
		tableTemplate = _.template($('#table-template').html());
		$queryTbl.html(tableTemplate);
		$('#result_tbl').html("");

		// Each row data
		for (i = 0; i < data_len - 1; i += 1) {
			trTemplate = _.template(
				$('#tr-template').html(),
				{
					'data': eason.data[i],
				}
			);
			$queryTbl.find('tbody').append(trTemplate);
		}

		// Sum or Result
		if (eason.result !== "success"){
			trResultTemplate = _.template($('#tr-result-template').html(), {'data': eason.result});
			$('#result_tbl').append(trResultTemplate);
		}
		else{
			trSumTemplate = _.template($('#tr-sum-template').html(), {'data': eason.data[data_len - 1]});
			$('#result_tbl').append(trSumTemplate);
		}

		$("#query_tbl").tablesorter({
			theme : 'blue',
			sortList : [[1, 0]],

			headerTemplate : '{content}{icon}',

			widgets : ["columns"],
			widgetOptions : {
				columns : [ "primary" ]
			}
		});

	}

	function classicSubmit(event) {
		event.preventDefault();
		var ajaxData;

		if (!$form.data('bootstrapValidator').isValid()) {
			return false;
		}

		ajaxData = {
			item: $('#item').val(),
			user: $('#user').val(),
			quantity: $('#quantity').val(),
			sugar: $('#sugar').val(),
			ice: $('#ice').val(),
			shop: $('#shop').val(),
			note: $('#note').val()
		};
		console.log(ajaxData);
		$.ajax({
			url: '/order/post/',
			type: 'POST',
			data: JSON.stringify(ajaxData),
			contentType: 'application/json; charset=utf-8',
			dataType: 'json',
			success: show_order
		});
		return false;
	}

	function bindEvents() {
		$('.datepicker').pickadate({
			format: 'yyyy-mm-dd',
			formatSubmit: 'yyyy-mm-dd',
			editable: true
		});
		$('#date_begin').change(function () {
			$form.data('bootstrapValidator').revalidateField('date_begin');
		});
		$('#date_end').change(function () {
			$form.data('bootstrapValidator').revalidateField('date_end');
		});
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
				date_begin: {
					validators: {
						date: {
							format: 'YYYY-MM-DD',
							message: 'The value is not a valid date'
						}
					}
				},
				date_end: {
					validators: {
						date: {
							format: 'YYYY-MM-DD',
							message: 'The value is not a valid date'
						}
					}
				},
			}
		}).on('success.form.bv', classicSubmit);
	}

	function init() {
		preloadResources();
		render();
		setValidate();
		bindEvents();
	}

	// Entry point
	init();

}(jQuery));
// vim: set tabstop=4 noexpandtab shiftwidth=4 softtabstop=4:
