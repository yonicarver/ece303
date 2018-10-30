classdef control_panel < matlab.apps.AppBase

    % Properties that correspond to app components
    properties (Access = public)
        UIFigure                     matlab.ui.Figure
        button_start                 matlab.ui.control.Button
        button_stop                  matlab.ui.control.Button
        SpeedmphLabel                matlab.ui.control.Label
        gauge_speed                  matlab.ui.control.Gauge
        BatteryLifeLabel             matlab.ui.control.Label
        gauge_battery_life           matlab.ui.control.SemicircularGauge
        LowCoolantWarningLabel       matlab.ui.control.Label
        lamp_low_coolant             matlab.ui.control.Lamp
        HighTemperatureWarningLabel  matlab.ui.control.Label
        lamp_high_temperature        matlab.ui.control.Lamp
        EStopStatusLabel             matlab.ui.control.Label
        lamp_estop                   matlab.ui.control.Lamp
        lamp_start_status            matlab.ui.control.Lamp
        lamp_stop_status             matlab.ui.control.Lamp
        button_reset                 matlab.ui.control.Button
        button_distance_traveled     matlab.ui.control.Button
        Slider1Label                 matlab.ui.control.Label
        Slider1                      matlab.ui.control.Slider
        label_distance_traveled      matlab.ui.control.Label
        TireRPMLabel                 matlab.ui.control.Label
        button_tire_rpm              matlab.ui.control.Button
        NormalOperationLabel         matlab.ui.control.Label
        lamp_normal_op               matlab.ui.control.Lamp
        TextArea                     matlab.ui.control.TextArea
    end

    
    properties (Access = private)
%         Property1 % flags
        estop_flag = 0;     % initialize estop_flag - 1 if estop button is pressed, 0 if estop button is not pressed
        
    end
    
    methods (Access = private)
        
    end
    
    methods (Access = public)
        
    end
    

    methods (Access = private)

        % Code that executes after component creation
        function startupFcn(app, estop_status, main_relay, load_cell, optical_counter, temp, coolant, led_normal_op, led_high_temp, led_low_coolant)
            % STARTUP FUNCTION
            
%             app.estop_flag = 0; % in estop
            app.lamp_estop.Color = 'red';
            app.lamp_normal_op.Color = 'white';
            app.gauge_speed.Value = optical_counter;
%             app.TextArea.Value = estop_status + main_relay + load_cell + optical_counter + temp + coolant + led_normal_op + led_high_temp + led_low_coolant;
            app.button_distance_traveled.Text = string(estop_status);
%             app.lab.Text = optical_counter;
        end

        % Button pushed function: button_start
        function button_startButtonPushed(app, event)
            if (app.lamp_estop.Color == [1 1 0])    % if estop lamp is red
                app.lamp_estop.Color = 'green';
                app.lamp_normal_op.Color = 'white';
            elseif (app.estop_flag == 1)
                app.lamp_estop.Color = 'white';
                app.estop_flag = 0;     % estop is cleared
            
            else
                app.lamp_estop.Color = 'blue';
            end
        end

        % Button pushed function: button_stop
        function button_stopButtonPushed(app, event)
            app.lamp_estop.Color = 'red';
            app.estop_flag = 1;     % estop button is pressed
            app.lamp_normal_op.Color = 'black';
        end

        % Button pushed function: button_reset
        function button_resetButtonPushed(app, event)
%             estop_reset_flag = true;
            app.lamp_estop.Color = 'yellow';
            app.estop_flag = 0;     % estop is cleared
        end

        % Callback function
        function Slider1ValueChanged(app, event)
            % testing purposes only
            value = app.Slider1.Value;
            app.gauge_speed.Value = value;
        end

        % Value changed function: Slider1
        function Slider1ValueChanged2(app, event)
            % testing purposes only
            value = app.Slider1.Value;
%             app.gauge_speed.Value = value;
%             app.button_distance_traveled.Text = string(value);
        end

        % Button pushed function: button_tire_rpm
        function button_tire_rpmButtonPushed(app, event)
            
        end
    end

    % App initialization and construction
    methods (Access = private)

        % Create UIFigure and components
        function createComponents(app)

            % Create UIFigure
            app.UIFigure = uifigure;
            app.UIFigure.Position = [100 100 640 480];
            app.UIFigure.Name = 'UI Figure';

            % Create button_start
            app.button_start = uibutton(app.UIFigure, 'push');
            app.button_start.ButtonPushedFcn = createCallbackFcn(app, @button_startButtonPushed, true);
            app.button_start.BackgroundColor = [0 1 0];
            app.button_start.Position = [452 357 58 41];
            app.button_start.Text = 'START';

            % Create button_stop
            app.button_stop = uibutton(app.UIFigure, 'push');
            app.button_stop.ButtonPushedFcn = createCallbackFcn(app, @button_stopButtonPushed, true);
            app.button_stop.BackgroundColor = [1 0 0];
            app.button_stop.Position = [452 301 58 41];
            app.button_stop.Text = 'STOP';

            % Create SpeedmphLabel
            app.SpeedmphLabel = uilabel(app.UIFigure);
            app.SpeedmphLabel.HorizontalAlignment = 'center';
            app.SpeedmphLabel.Position = [75 261 75 22];
            app.SpeedmphLabel.Text = 'Speed (mph)';

            % Create gauge_speed
            app.gauge_speed = uigauge(app.UIFigure, 'circular');
            app.gauge_speed.Limits = [0 200];
            app.gauge_speed.MajorTicks = [0 20 40 60 80 100 120 140 160 180 200];
            app.gauge_speed.Position = [38 290 150 150];

            % Create BatteryLifeLabel
            app.BatteryLifeLabel = uilabel(app.UIFigure);
            app.BatteryLifeLabel.HorizontalAlignment = 'center';
            app.BatteryLifeLabel.Position = [80 146 66 22];
            app.BatteryLifeLabel.Text = 'Battery Life';

            % Create gauge_battery_life
            app.gauge_battery_life = uigauge(app.UIFigure, 'semicircular');
            app.gauge_battery_life.Position = [53 170 120 65];

            % Create LowCoolantWarningLabel
            app.LowCoolantWarningLabel = uilabel(app.UIFigure);
            app.LowCoolantWarningLabel.HorizontalAlignment = 'right';
            app.LowCoolantWarningLabel.Position = [405 174 120 22];
            app.LowCoolantWarningLabel.Text = 'Low Coolant Warning';

            % Create lamp_low_coolant
            app.lamp_low_coolant = uilamp(app.UIFigure);
            app.lamp_low_coolant.Position = [540 174 20 20];
            app.lamp_low_coolant.Color = [0 0 1];

            % Create HighTemperatureWarningLabel
            app.HighTemperatureWarningLabel = uilabel(app.UIFigure);
            app.HighTemperatureWarningLabel.HorizontalAlignment = 'right';
            app.HighTemperatureWarningLabel.Position = [377 147 148 22];
            app.HighTemperatureWarningLabel.Text = 'High Temperature Warning';

            % Create lamp_high_temperature
            app.lamp_high_temperature = uilamp(app.UIFigure);
            app.lamp_high_temperature.Position = [540 147 20 20];
            app.lamp_high_temperature.Color = [1 0 0];

            % Create EStopStatusLabel
            app.EStopStatusLabel = uilabel(app.UIFigure);
            app.EStopStatusLabel.HorizontalAlignment = 'right';
            app.EStopStatusLabel.Position = [393 418 80 22];
            app.EStopStatusLabel.Text = 'E-Stop Status';

            % Create lamp_estop
            app.lamp_estop = uilamp(app.UIFigure);
            app.lamp_estop.Position = [488 418 20 20];

            % Create lamp_start_status
            app.lamp_start_status = uilamp(app.UIFigure);
            app.lamp_start_status.Position = [523 367 20 20];

            % Create lamp_stop_status
            app.lamp_stop_status = uilamp(app.UIFigure);
            app.lamp_stop_status.Position = [523 311 20 20];

            % Create button_reset
            app.button_reset = uibutton(app.UIFigure, 'push');
            app.button_reset.ButtonPushedFcn = createCallbackFcn(app, @button_resetButtonPushed, true);
            app.button_reset.BackgroundColor = [1 1 0];
            app.button_reset.Position = [377 301 58 41];
            app.button_reset.Text = 'RESET';

            % Create button_distance_traveled
            app.button_distance_traveled = uibutton(app.UIFigure, 'push');
            app.button_distance_traveled.Position = [472 86 75 22];
            app.button_distance_traveled.Text = '';

            % Create Slider1Label
            app.Slider1Label = uilabel(app.UIFigure);
            app.Slider1Label.HorizontalAlignment = 'right';
            app.Slider1Label.Position = [2 54 43 22];
            app.Slider1Label.Text = 'Slider1';

            % Create Slider1
            app.Slider1 = uislider(app.UIFigure);
            app.Slider1.ValueChangedFcn = createCallbackFcn(app, @Slider1ValueChanged2, true);
            app.Slider1.Position = [66 63 150 3];

            % Create label_distance_traveled
            app.label_distance_traveled = uilabel(app.UIFigure);
            app.label_distance_traveled.Position = [332 86 141 22];
            app.label_distance_traveled.Text = 'Distance Traveled (miles)';

            % Create TireRPMLabel
            app.TireRPMLabel = uilabel(app.UIFigure);
            app.TireRPMLabel.Position = [412 65 56 22];
            app.TireRPMLabel.Text = 'Tire RPM';

            % Create button_tire_rpm
            app.button_tire_rpm = uibutton(app.UIFigure, 'push');
            app.button_tire_rpm.ButtonPushedFcn = createCallbackFcn(app, @button_tire_rpmButtonPushed, true);
            app.button_tire_rpm.Position = [472 63 75 22];
            app.button_tire_rpm.Text = '';

            % Create NormalOperationLabel
            app.NormalOperationLabel = uilabel(app.UIFigure);
            app.NormalOperationLabel.HorizontalAlignment = 'right';
            app.NormalOperationLabel.Position = [425 200 100 22];
            app.NormalOperationLabel.Text = 'Normal Operation';

            % Create lamp_normal_op
            app.lamp_normal_op = uilamp(app.UIFigure);
            app.lamp_normal_op.Position = [540 200 20 20];
            app.lamp_normal_op.Color = [1 1 1];

            % Create TextArea
            app.TextArea = uitextarea(app.UIFigure);
            app.TextArea.Position = [2 453 639 28];
        end
    end

    methods (Access = public)

        % Construct app
        function app = control_panel

            % Create and configure components
            createComponents(app)

            % Register the app with App Designer
            registerApp(app, app.UIFigure)

            % Execute the startup function
            runStartupFcn(app, @startupFcn)

            if nargout == 0
                clear app
            end
        end

        % Code that executes before app deletion
        function delete(app)

            % Delete UIFigure when app is deleted
            delete(app.UIFigure)
        end
    end
end