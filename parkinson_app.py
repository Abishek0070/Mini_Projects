import streamlit as st
import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import accuracy_score
import joblib

# ------------------------
# Load and prepare dataset
# ------------------------
@st.cache_data
def load_data():
    url = "https://archive.ics.uci.edu/ml/machine-learning-databases/parkinsons/parkinsons.data"
    df = pd.read_csv(url)
    return df

df = load_data()

X = df.drop(columns=["name", "status"])
y = df["status"]

scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

X_train, X_test, y_train, y_test = train_test_split(X_scaled, y, test_size=0.2, random_state=42)

model = RandomForestClassifier(random_state=42)
model.fit(X_train, y_train)

accuracy = accuracy_score(y_test, model.predict(X_test))

# ------------------------
# Streamlit UI
# ------------------------
st.title("🧠 Parkinson's Disease Prediction App")
st.write("This is a **demo app** using ML to detect Parkinson’s disease (educational only).")
st.write(f"Model Accuracy on Test Data: **{accuracy:.2f}**")

st.sidebar.header("Input Patient Data")

# Dynamic inputs
input_data = []
for col in X.columns:
    val = st.sidebar.number_input(f"{col}", float(df[col].min()), float(df[col].max()), float(df[col].mean()))
    input_data.append(val)

input_array = np.array(input_data).reshape(1, -1)
input_scaled = scaler.transform(input_array)

if st.sidebar.button("Predict"):
    prediction = model.predict(input_scaled)[0]
    prob = model.predict_proba(input_scaled)[0][1]
    if prediction == 1:
        st.error(f"⚠️ High likelihood of Parkinson’s (Probability: {prob:.2f})")
    else:
        st.success(f"✅ Low likelihood of Parkinson’s (Probability: {prob:.2f})")

st.caption("⚠️ Disclaimer: This app is for **educational purposes only** and not for medical use.")
