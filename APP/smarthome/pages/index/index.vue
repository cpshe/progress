<template>
	<view class="wrap">
		<view class="dev-area">

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">温度</view>
					<image class="dev-logo" src="../../static/temp.png" mode="/"></image>
				</view>
				<view class="dev-data">{{Temp}}℃</view>
			</view>
			<view class="dev-cart">
				<view class="">
					<view class="dev-name">湿度</view>
					<image class="dev-logo" src="../../static/humi.png" mode="/"></image>
				</view>
				<view class="dev-data">{{Humi}}℃</view>
			</view>
			<view class="dev-cart">
				<view class="">
					<view class="dev-name">光照度</view>
					<image class="dev-logo" src="../../static/light.png" mode="/"></image>
				</view>
				<view class="dev-data">{{Light}}℃</view>
			</view>
			<view class="dev-cart">
				<view class="">
					<view class="dev-name">台灯</view>
					<image class="dev-logo" src="../../static/led.png" mode="/"></image>
				</view>
				<switch :checked="led" @change="onLedSwitch" />
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
				Temp: '',
				Humi: '',
				Light: '',
				led: false,
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
						product_id: 'r5H174uJB6',
						device_name: 'dev'
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: (res) => {
						console.log(res.data);
						this.Humi = res.data.data[0].value;
						this.Light = res.data.data[1].value;
						this.Temp = res.data.data[2].value;
						this.led = res.data.data[3].value === 'true';
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
						product_id: 'r5H174uJB6',
						device_name: 'dev',
						params: {
							"led": value
						}
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: () => {
						console.log('LED' + (value ? 'ON' : 'OFF') + '!');
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