<template>
	<view class="wrap">
		<view class="dev-area">

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">解锁</view>
					<image class="dev-logo" src="../../static/open.png" mode="/"></image>
				</view>
				<switch :checked="led" @change="onLedSwitch" />
			</view>

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">报警</view>
					<image class="dev-logo" src="../../static/beep.png" mode="/"></image>
				</view>
				<switch :checked="beep" @change="onBeepSwitch" />
			</view>

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">生成临时密码</view>
					<image class="dev-logo" src="../../static/generate.png" mode="/"></image>
				</view>
				<switch :checked="generate_flag" @change="onGeneratePwdSwitch" />
			</view>
			
			<view class="dev-cart">
				<view class="">
					<view class="dev-name">临时密码</view>
					<image class="dev-logo" src="../../static/temperarypwd.png" mode="/"></image>
				</view>
				<view class="dev-data">{{generate_pwd}}</view>
			</view>
			
		</view>
	</view>
</template>

<script>
	const {
		createCommonToken
	} = require('@/key.js')
	export default {
		data() {
			return {
				generate_pwd: '',
				led: false,
				beep: false,
				generate_flag: false,
				token: '',
			}
		},
		onLoad() {
			const params = {
				author_key: 'EWMs7JXnt6aiTJsR7TOQsFTQmFKu+hGw0BLeNdX+IJCxbc6RSuAg9xHbP8+HjnSUNrDbszDNIXkeiAU06BNhaA==',
				version: '2022-05-01',
				user_id: '400641',
			}

			this.token = createCommonToken(params);
			console.log(this.token)
		},
		onShow() {
			this.fetchDevData();
			setInterval(() => {
				this.fetchDevData();
			}, 3000)
		},
		methods: {
			fetchDevData() {
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/query-device-property', //仅为示例，并非真实接口地址。
					method: 'GET',
					data: {
						product_id: '9A7a7Sj8GY',
						device_name: 'lock'
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: (res) => {
						console.log(res.data);
						this.beep = res.data.data[0].value === 'true';
						this.generate_flag = res.data.data[2] === 'true';
						this.generate_pwd = res.data.data[3].value;
						this.led = res.data.data[4].value === 'true';
					}
				});
			},
			onLedSwitch(event) {
				console.log(event.detail.value);
				let value = event.detail.value;
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property', //仅为示例，并非真实接口地址。
					method: 'POST',
					data: {
						product_id: '9A7a7Sj8GY',
						device_name: 'lock',
						params: {
							"led": value,
						}
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: () => {
						console.log('LED' + (value ? 'ON' : 'OFF') + '!');
					}
				});
			},
			onBeepSwitch(event) {
				console.log(event.detail.value);
				let value = event.detail.value;
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property', //仅为示例，并非真实接口地址。
					method: 'POST',
					data: {
						product_id: '9A7a7Sj8GY',
						device_name: 'lock',
						params: {
							"beep": value,
						}
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: () => {
						console.log('BEEP' + (value ? 'ON' : 'OFF') + '!');
					}
				});
			},
			onGeneratePwdSwitch(event) {
				console.log(event.detail.value);
				let value = event.detail.value;
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property', //仅为示例，并非真实接口地址。
					method: 'POST',
					data: {
						product_id: '9A7a7Sj8GY',
						device_name: 'lock',
						params: {
							"generate_flag": value,
						}
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: () => {
						console.log('GENERATE_PWD' + (value ? 'ON' : 'OFF') + '!');
					}
				});
			}
		}
	}
</script>

<style>
	.wrap {
		padding: 30rpx;

	}

	.dev-area {
		display: flex;
		justify-content: space-between;
		flex-wrap: wrap;
	}

	.dev-cart {
		height: 150rpx;
		width: 320rpx;
		border-radius: 30rpx;
		margin-top: 30rpx;
		display: flex;
		justify-content: space-around;
		align-items: center;
		box-shadow: 0 0 15rpx #ccc;
	}

	.dev-name {
		font-size: 20rpx;
		text-align: center;
		color: #6d6d6d;
	}

	.dev-logo {
		width: 70rpx;
		height: 70rpx;
		margin-top: 10rpx;
	}

	.dev-data {
		font-size: 50rpx;
		color: #6d6d6d;
	}
</style>