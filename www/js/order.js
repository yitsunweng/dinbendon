/*jslint browser:true */
/*jslint es5: true */
/*jslint nomen: true */
/*jslint white: true */
/*global $, jQuery, alert, console, _, FB*/

(function ($) {
	'use strict';

	var $form;

	$form = $('#form');

	function prequery(){
		$.ajax({
			url: '/orderd/post/',
			type: 'POST',
			contentType: 'application/json; charset=UTF-8',
			dataType: 'json',
			success: query
		});
	}

	function render() {
		var template;
		template = _.template($('#form-template').html());
		$form.html(template);
		console.log('Render form template complete');
		prequery();
	}

	function query(sql){
		console.log('get query result');
		var i, data_len, trTemplate, tableTemplate,	$queryTbl, queryTemplate, trSumTemplate;
		data_len = sql.data.length;
		$queryTbl = $('#query_div');
		tableTemplate = _.template($('#table-template').html());
		$queryTbl.html(tableTemplate);
		$('#result_tbl').html("");

		// Each row data
		for (i = 0; i < data_len-1; i += 1) {
			trTemplate = _.template($('#tr-template').html(), {'data': sql.data[i]});
			$queryTbl.find('tbody').append(trTemplate);
		}

		// Query Result
		if (sql.result !== "Success"){
			queryTemplate = _.template($('#query-result-template').html(), {'data':sql.result});
			$('#result_tbl').append(queryTemplate);
		}
		else{
			trSumTemplate = _.template($('#tr-sum-template').html(), {'sum': sql.data[data_len - 1]});
			$('#result_tbl').append(trSumTemplate);
		}

		$("#query_tbl").tablesorter({
			theme : 'blue',
			sortList : [[0, 0]],

			headerTemplate : '{content}{icon}',

			widgets : ["columns"],
			widgetOptions : {
				columns : [ "primary" ]
			}
		});

	}

	function show_order(insert) {
		console.log('get insert result');
		$('#submit').removeAttr('disabled');

		prequery();

		// Insert Result
		console.log(insert.result);
		if (insert.result == "Fail")
			alert('Insert ' + insert.result);
	}

	function classicSubmit(event) {
		event.preventDefault();
		var ajaxData;

		if (!$form.data('bootstrapValidator').isValid()) {
			return false;
		}

		ajaxData = {
			user: $('#user').val(),
			item: $('#item').val(),
			price: $('#price').val(),
			quantity: $('#quantity').val(),
			sugar: $('#sugar').val(),
			ice: $('#ice').val(),
			note: $('#note').val()
		};
		console.log(ajaxData);
		$.ajax({
			url: '/order/post/',
			type: 'POST',
			data: JSON.stringify(ajaxData),
			contentType: 'application/json; charset=UTF-8',
			dataType: 'json',
			success: show_order
		});
		return false;
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
				item:		{validators: {notEmpty: {message: '< ! > empty'},}},
				user:		{validators: {notEmpty: {message: '< ! > empty'},}},
				price:		{validators: {notEmpty: {message: '< ! > empty'},}},
				quantity:	{validators: {notEmpty: {message: '< ! > empty'},}},
			}
		}).on('success.form.bv', classicSubmit);
	}

	function init() {
		preloadResources();
		render();
		setValidate();
	}

	init();

}(jQuery));
// vim: set tabstop=4 noexpandtab shiftwidth=4 softtabstop=4:
